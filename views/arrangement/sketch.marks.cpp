// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../control.hpp"
#include "../../inventory.hpp"
#include "../../peaks.hpp"
#include "../../shape.hpp"
#include "../../timeline.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::ARRANGEMENT::Box;
using VIEWS::ARRANGEMENT::Pip;

struct Mark {
  Whole at = 0, run = 0;
  Float down = 0.0f, deep = 0.0f;
};

constexpr Whole MOST = 96;
constexpr Float FULL = 1.0f;
constexpr Float THIN = 0.10f;
constexpr Float AIR = 0.20f;
constexpr Float MID = 0.5f;
constexpr Float TICK = 0.006f;
constexpr Float LINE = 0.02f;
constexpr Float HALF = 0.5f;
constexpr Whole PASSES = 8;
constexpr Whole ALONG = 8;
constexpr Float WHOLE = 1.0f;

auto strode(Whole count) -> Whole {
  return count > ::MOST ? (count + ::MOST - 1) / ::MOST : 1;
}

auto banded(Float value) -> Mark {
  const Float run = ::FULL - ::AIR * 2.0f - ::THIN;
  return {0, 0, ::AIR + run * (1.0f - std::clamp(value, 0.0f, 1.0f)), ::THIN};
}

auto marked(const Score &score) -> Vector<Mark> {
  Whole low = 0, high = 0;
  Flag any = false;
  for (const Note &note : score.notes) {
    low = any ? std::min(low, note.pitch) : note.pitch;
    high = any ? std::max(high, note.pitch) : note.pitch;
    any = true;
  }
  Vector<Mark> marks;
  const Float run = Float(high > low ? high - low : 1);
  const Whole stride = ::strode(score.notes.size());
  for (Whole one = 0; one < score.notes.size(); one += stride) {
    const Note &note = score.notes[one];
    Mark mark = ::banded(high > low ? Float(note.pitch - low) / run : ::MID);
    mark.at = note.at;
    mark.run = note.length;
    marks.push_back(mark);
  }
  return marks;
}

auto waved(Float peak) -> Mark {
  const Float run = ::FULL - ::AIR * 2.0f;
  const Float deep = std::max(::LINE, run * std::clamp(peak, 0.0f, 1.0f));
  return {0, 0, ::MID - deep * ::HALF, deep};
}

auto marked(Whole stock, const Take &take) -> Vector<Mark> {
  Vector<Mark> marks;
  if (take.lanes.empty() || take.lanes[0].empty()) return marks;
  const Whole frames = take.lanes[0].size();
  const Whole window = frames / ::MOST + 1;
  const PEAKS::Level &level = PEAKS::GET::level(stock);
  for (Whole at = 0; at < frames; at += window) {
    const PEAKS::Peak peak = PEAKS::peak(take, level, at, window);
    Mark mark = ::waved(std::max(-peak.least, peak.most));
    mark.at = at;
    mark.run = window;
    marks.push_back(mark);
  }
  return marks;
}

auto along(Whole kept) -> Whole {
  const Whole share = kept == 0 ? ::ALONG : ::MOST / kept;
  return std::clamp(share, Whole(1), ::ALONG);
}

template <typename Node>
auto shaped(const Vector<Node> &nodes, Float reach) -> Vector<Mark> {
  Vector<Mark> marks;
  const Whole stride = ::strode(nodes.size());
  const Whole pieces = ::along((nodes.size() + stride - 1) / stride);
  for (Whole one = 0; one < nodes.size(); one += stride) {
    const Whole next = one + stride;
    const Float value = nodes[one].value / reach;
    const Whole parts =
      next < nodes.size() && nodes[one].shape != SHAPE::HOLD ? pieces : 1;
    const Whole run = next < nodes.size() ? nodes[next].at - nodes[one].at : 0;
    const Float rise =
      next < nodes.size() ? nodes[next].value / reach - value : 0.0f;
    for (Whole part = 0; part < parts; ++part) {
      Mark mark = ::banded(
        value + rise * SHAPE::reading(nodes[one], Float(part) / Float(parts)));
      mark.at = nodes[one].at + run * part / parts;
      mark.run = run * (part + 1) / parts - run * part / parts;
      marks.push_back(mark);
    }
  }
  return marks;
}

auto marked(const Vector<Choice> &choices) -> Vector<Mark> {
  Vector<Mark> marks;
  const Whole stride = ::strode(choices.size());
  for (Whole one = 0; one < choices.size(); one += stride) {
    Mark mark = ::banded(::MID);
    mark.at = choices[one].at;
    marks.push_back(mark);
  }
  return marks;
}

auto wrapped(const Vector<Mark> &marks, Whole from, Whole carried, Whole held)
  -> Vector<Pip> {
  Vector<Pip> pips;
  if (carried == 0 || held == 0) return pips;
  for (const Mark &mark : marks)
    for (Whole at = TIMELINE::opened(mark.at, from, carried); at < held;
         at += carried) {
      pips.push_back(
        {Float(at) / Float(held),
         std::max(::TICK, Float(mark.run) / Float(held)), mark.down,
         mark.deep});
      if (pips.size() >= ::MOST * ::PASSES) return pips;
    }
  return pips;
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::pips(const Box &box) -> Vector<Pip> {
  const Inventory &pool = INVENTORY::held();
  if (box.stock >= pool.stocks.size()) return {};
  const Stock &stock = pool.stocks[box.stock];
  const Whole held = stocked(box, box.frames);
  const Whole carried = INVENTORY::length(stock);
  Vector<Mark> marks;
  if (stock.kind == KIND::AUDIO)
    marks = ::marked(box.stock, stock.take);
  else if (stock.kind == KIND::LOGIC)
    marks = ::shaped(stock.curve.points, ::WHOLE);
  else if (stock.kind == KIND::CONTROL)
    marks = ::shaped(stock.turns, Float(CONTROL::LOUDEST));
  else if (stock.kind == KIND::PROGRAM)
    marks = ::marked(stock.choices);
  else
    marks = ::marked(stock.score);
  return ::wrapped(marks, box.from, carried, held);
}

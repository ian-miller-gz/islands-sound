// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../inventory.hpp"
#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "../../transport.hpp"
#include "../lane/lane.face.hpp"
#include "../views.marks.hpp"
#include "trace.internal.hpp"

namespace {
using namespace SOUND;
namespace TRACE = SOUND::VIEWS::TRACE;
namespace LANE = SOUND::VIEWS::LANE;
using Lane = ARRANGEMENT::TRACK::Lane;

constexpr STRING::Hot COUNT = "clips {}";
constexpr STRING::Hot LINE = "clip {} {} {} {} {} bars {}{}";
constexpr STRING::Hot MARKED = " marked";
constexpr STRING::Hot UNMARKED = "";
constexpr STRING::Hot PLACE = "{}.{}";
constexpr Whole FIRST = 1;
constexpr Whole LEAST = 1;
constexpr Float UNBARRED = 0.0f;

auto lanes() -> const Vector<Lane> * {
  const Whole track = TRACE::attended();
  if (track == NONE) return nullptr;
  return &TIMELINE::held().tracks[track].lanes;
}

auto section(Whole at) -> TRANSPORT::Section {
  return VIEWS::standing(TRANSPORT::framed(TRANSPORT::sections()), at);
}

auto placed(const LANE::Clip &clip) -> String {
  const TRANSPORT::Section one = ::section(clip.at);
  if (one.bar == 0) return {};
  const Whole into = clip.at - one.at;
  const Whole beat =
    std::max(one.bar / std::max(one.numerator, ::LEAST), ::LEAST);
  return std::format(
    ::PLACE, one.bars + into / one.bar + ::FIRST,
    into % one.bar / beat + ::FIRST);
}

auto bars(const LANE::Clip &clip) -> Float {
  const TRANSPORT::Section one = ::section(clip.at);
  return one.bar == 0 ? ::UNBARRED : Float(clip.frames) / Float(one.bar);
}

auto named(const LANE::Clip &clip) -> String {
  const Vector<Stock> &stocks = INVENTORY::held().stocks;
  return clip.stock < stocks.size() ? stocks[clip.stock].name : String();
}

auto before(const TRACE::Clipped &one, const TRACE::Clipped &two) -> Flag {
  return one.at != two.at ? one.at < two.at : one.lane < two.lane;
}

}  // namespace

auto SOUND::VIEWS::TRACE::clipped() -> Vector<Clipped> {
  const Vector<Lane> *lanes = ::lanes();
  if (lanes == nullptr) return {};
  const LANE::Clip *scoped = LANE::GET::scoped();
  Vector<Clipped> rows;
  for (Whole lane = 0; lane < lanes->size(); ++lane)
    for (Whole clip = 0; clip < (*lanes)[lane].clips.size(); ++clip) {
      const LANE::Clip &one = (*lanes)[lane].clips[clip];
      rows.push_back(
        {clip, lane, (*lanes)[lane].kind, one.at, ::named(one), ::placed(one),
         ::bars(one), &one == scoped});
    }
  std::ranges::stable_sort(rows, ::before);
  return rows;
}

void SOUND::VIEWS::TRACE::clips(SHELL::Session &session) {
  const Vector<Clipped> rows = clipped();
  session.print(std::format(::COUNT, rows.size()));
  for (const Clipped &row : rows)
    session.print(std::format(
      ::LINE, row.clip, row.lane, KIND::spoken(row.kind), row.name, row.place,
      row.bars, row.scoped ? ::MARKED : ::UNMARKED));
}

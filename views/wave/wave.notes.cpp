// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "wave.notes.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;
namespace LANE = SOUND::VIEWS::LANE;

constexpr Float MID = 0.5f;

struct Entry {
  Whole stock = NONE;
  Float ratio = 0.0f;
  Integer cents = 0;
  Whole stirred = NONE;
  Vector<CONTOUR::Found> founds;
};

Vector<Entry> entries;

auto keyed(const Entry &entry, const WAVE::Clip &clip) -> Flag {
  return entry.stock == clip.stock && entry.ratio == clip.ratio &&
         entry.cents == clip.cents;
}

auto named(const Entry &entry) -> Flag {
  for (const ARRANGEMENT::Track &track : TIMELINE::held().tracks)
    for (const ARRANGEMENT::TRACK::Lane &lane : track.lanes)
      for (const WAVE::Clip &clip : lane.clips)
        if (::keyed(entry, clip)) return true;
  return false;
}

auto stale(const Entry &entry) -> Flag {
  return entry.stirred != INVENTORY::stirred(entry.stock) || !::named(entry);
}

auto valued(Float pitch, const WAVE::Band &band) -> Float {
  return band.high > band.low ? (pitch - band.low) / (band.high - band.low)
                              : ::MID;
}

auto made(const WAVE::Clip &clip, Whole stirred)
  -> const Vector<CONTOUR::Found> & {
  std::erase_if(::entries, [&clip](const Entry &entry) {
    return ::keyed(entry, clip) || ::stale(entry);
  });
  WAVE::Clip plain = clip;
  plain.tunes.clear();
  const TIMELINE::Derived source = TIMELINE::derived(plain.stock, plain);
  Entry entry = {
    .stock = clip.stock,
    .ratio = clip.ratio,
    .cents = clip.cents,
    .stirred = stirred,
    .founds = {}};
  if (source.take != nullptr)
    entry.founds = CONTOUR::noted(CONTOUR::traced(*source.take));
  return ::entries.emplace_back(std::move(entry)).founds;
}

void bars(
  const WAVE::Clip &clip, Float west, Float east, Vector<WAVE::Bar> &bars) {
  const Vector<WAVE::Sung> sung = WAVE::sung(clip);
  if (sung.empty()) return;
  const WAVE::Band band = WAVE::banded(sung);
  for (const WAVE::Sung &one : sung)
    for (Whole at : TIMELINE::placed(clip, {one.found.at})) {
      const Whole run = std::min(one.found.frames, clip.at + clip.frames - at);
      if (Float(at) > east || Float(at + run) < west) continue;
      bars.push_back(
        {at, run, Whole(std::lround(one.now)), ::valued(one.now, band),
         ::valued(one.was, band)});
    }
}

}  // namespace

auto SOUND::VIEWS::WAVE::notes(const Clip &clip) -> Vector<CONTOUR::Found> {
  const Whole stirred = INVENTORY::stirred(clip.stock);
  for (const ::Entry &entry : ::entries)
    if (::keyed(entry, clip) && entry.stirred == stirred) return entry.founds;
  return ::made(clip, stirred);
}

auto SOUND::VIEWS::WAVE::bars(Float west, Float east) -> Vector<Bar> {
  if (LANE::faced(laned()).notes == 0) return {};
  Vector<Bar> bars;
  for (const WAVE::Clip &clip : placed()) ::bars(clip, west, east, bars);
  return bars;
}

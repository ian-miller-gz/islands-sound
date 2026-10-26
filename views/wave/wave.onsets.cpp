// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../inventory.hpp"
#include "../../onset.hpp"
#include "../../timeline.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

struct Entry {
  Whole stock = NONE;
  Float ratio = 0.0f;
  Integer cents = 0;
  Float sense = 0.0f;
  Whole stirred = NONE;
  Vector<Whole> frames;
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

auto made(const WAVE::Clip &clip, Float sense, Whole stirred)
  -> const Vector<Whole> & {
  std::erase_if(::entries, [&clip](const Entry &entry) {
    return ::keyed(entry, clip) || ::stale(entry);
  });
  const TIMELINE::Derived source = TIMELINE::derived(clip.stock, clip);
  Entry entry = {
    .stock = clip.stock,
    .ratio = clip.ratio,
    .cents = clip.cents,
    .sense = sense,
    .stirred = stirred,
    .frames = {}};
  if (source.take != nullptr) entry.frames = ONSET::found(*source.take, sense);
  return ::entries.emplace_back(std::move(entry)).frames;
}

}  // namespace

auto SOUND::VIEWS::WAVE::onsets(const Clip &clip, Float sense)
  -> Vector<Whole> {
  const Whole stirred = INVENTORY::stirred(clip.stock);
  for (const ::Entry &entry : ::entries)
    if (
      ::keyed(entry, clip) && entry.sense == sense && entry.stirred == stirred)
      return TIMELINE::placed(clip, entry.frames);
  return TIMELINE::placed(clip, ::made(clip, sense, stirred));
}

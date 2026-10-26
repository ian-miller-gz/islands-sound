// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../stretch.hpp"
#include "timeline.derived.hpp"
#include "timeline.internal.hpp"

namespace {
using namespace SOUND;
using ARRANGEMENT::TRACK::LANE::Clip;
using ARRANGEMENT::TRACK::LANE::Tune;

struct Entry {
  Whole stock = NONE;
  Float ratio = 0.0f;
  Integer cents = 0;
  Vector<Tune> tunes;
  Whole stirred = NONE;
  Take take;
  PEAKS::Level level;
};

Vector<Entry> entries;

auto neutral(const Clip &clip) -> Flag {
  const Clip plain;
  return clip.ratio == plain.ratio && clip.cents == plain.cents &&
         clip.tunes.empty();
}

auto same(const Tune &one, const Tune &other) -> Flag {
  return one.at == other.at && one.frames == other.frames &&
         one.cents == other.cents;
}

auto keyed(const Entry &entry, Whole stock, const Clip &clip) -> Flag {
  return entry.stock == stock && entry.ratio == clip.ratio &&
         entry.cents == clip.cents &&
         std::ranges::equal(entry.tunes, clip.tunes, ::same);
}

auto spans(const Vector<Tune> &tunes) -> Vector<STRETCH::Span> {
  Vector<STRETCH::Span> spans;
  for (const Tune &tune : tunes)
    spans.push_back(
      {.at = tune.at, .frames = tune.frames, .cents = tune.cents});
  return spans;
}

auto derive(const Take &take, const Clip &clip) -> Take {
  if (clip.tunes.empty())
    return STRETCH::stretched(take, clip.ratio, clip.cents);
  return STRETCH::stretched(
    STRETCH::spliced(take, ::spans(clip.tunes)), clip.ratio, clip.cents);
}

auto named(const Entry &entry) -> Flag {
  for (const ARRANGEMENT::Track &track : TIMELINE::held().tracks)
    for (const ARRANGEMENT::TRACK::Lane &lane : track.lanes)
      for (const Clip &clip : lane.clips)
        if (::keyed(entry, clip.stock, clip)) return true;
  return false;
}

auto stale(const Entry &entry) -> Flag {
  return entry.stirred != INVENTORY::stirred(entry.stock) || !::named(entry);
}

auto made(Whole stock, const Clip &clip) -> const Entry & {
  std::erase_if(::entries, ::stale);
  const Take &take = INVENTORY::held().stocks[stock].take;
  Entry entry = {
    .stock = stock,
    .ratio = clip.ratio,
    .cents = clip.cents,
    .tunes = clip.tunes,
    .stirred = INVENTORY::stirred(stock),
    .take = ::derive(take, clip),
    .level = {}};
  entry.level = PEAKS::level(entry.take);
  return ::entries.emplace_back(std::move(entry));
}

}  // namespace

auto SOUND::TIMELINE::derived(Whole stock, const Clip &clip) -> Derived {
  const Inventory &pool = INVENTORY::held();
  if (stock >= pool.stocks.size()) return {};
  if (::neutral(clip))
    return {&pool.stocks[stock].take, &PEAKS::GET::level(stock)};
  for (const Entry &entry : ::entries)
    if (
      ::keyed(entry, stock, clip) && entry.stirred == INVENTORY::stirred(stock))
      return {&entry.take, &entry.level};
  const Entry &entry = ::made(stock, clip);
  return {&entry.take, &entry.level};
}

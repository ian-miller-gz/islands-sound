// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../arrangement.hpp"

namespace SOUND::TIMELINE::SHIFT {

constexpr STRING::Hot WORD = "shift";
constexpr Integer SEMITONE = 100;
constexpr Integer SPAN = 24;
constexpr Integer FINE = 50;

using Clip = ARRANGEMENT::TRACK::LANE::Clip;

struct Interval {
  Integer semitones = 0;
  Integer cents = 0;
};

auto held(Integer cents) -> Integer;

auto split(Integer cents) -> Interval;

auto joined(const Interval &interval) -> Integer;

auto shifted(const Clip &clip, Integer cents) -> Clip;

}  // namespace SOUND::TIMELINE::SHIFT

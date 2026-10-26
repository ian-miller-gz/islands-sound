// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <cmath>
#include <numbers>

#include "../arrangement.hpp"

namespace SOUND::FADE {

constexpr Float FULL = 1.0f;
constexpr Float HALF = 0.5f;
constexpr Float PI = std::numbers::pi_v<Float>;

constexpr STRING::Hot WORD = "fade";
constexpr STRING::Hot OFF = "off";
constexpr Whole FLOOR = RATE / 400;

struct Shape {
  STRING::Hot word;
  Float (*gain)(Float along);
};

constexpr Shape SHAPES[] = {
  {"line", [](Float along) -> Float { return along; }},
  {"fast",
   [](Float along) -> Float { return FULL - (FULL - along) * (FULL - along); }},
  {"slow", [](Float along) -> Float { return along * along; }},
  {"power", [](Float along) -> Float { return std::sin(along * PI * HALF); }},
  {"even",
   [](Float along) -> Float { return HALF - HALF * std::cos(along * PI); }},
};

using Clip = ARRANGEMENT::TRACK::LANE::Clip;
using Fade = ARRANGEMENT::TRACK::LANE::Fade;

struct End {
  STRING::Hot word;
  Fade Clip::*fade;
  Fade Clip::*other;
  Flag east;
};

constexpr End ENDS[] = {
  {"in", &Clip::in, &Clip::out, false}, {"out", &Clip::out, &Clip::in, true}};

auto shaped(Whole shape) -> const Shape &;

auto room(const Clip &clip, const End &end) -> Whole;

auto knee(const Clip &clip, const End &end) -> Whole;

auto held(Integer frames, Whole room) -> Whole;

auto gain(const Fade &fade, Whole along) -> Float;

auto gain(const Clip &clip, Whole offset) -> Float;

}  // namespace SOUND::FADE

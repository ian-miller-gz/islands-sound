// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <common.hpp>

#include <cartridge/plugin.hpp>

namespace SOUND::VIEWS::ROLL {

constexpr STRING::Hot PLAIN = "\U0001D158";
constexpr STRING::Hot HOLLOW = "\U0001D157";  // the half note's head and longer
constexpr STRING::Hot CROSS = "\U0001D143";
constexpr STRING::Hot DIAMOND = "\U0001D1BA";

struct Drum {
  Whole from = 0, to = 0;
  Integer place = 0;
  STRING::Hot head = PLAIN, mark = "", name = "";
  Flag foot = false;
};

namespace GET {

auto drum(Whole note) -> const Drum *;

auto drums() -> Vector<AUDIO::PLUGIN::Voice>;

}  // namespace GET

auto headed(Whole note) -> STRING::Hot;

auto marked(Whole note) -> STRING::Hot;

}  // namespace SOUND::VIEWS::ROLL

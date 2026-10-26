// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <cartridge/plugin.hpp>

#include "../score.hpp"

namespace SOUND {
struct Address;
struct Inventory;
}  // namespace SOUND

namespace SOUND::TIMELINE {
struct Sent;
struct Governed;
}  // namespace SOUND::TIMELINE

namespace SOUND::RENDER {

auto mapped(Whole track, Whole lane) -> String;

auto edged(const TIMELINE::Sent &edge, Whole at) -> AUDIO::PLUGIN::Event;

auto sent(
  const Inventory &pool, Whole from, Whole frames,
  const Vector<String> &claims) -> Vector<TIMELINE::Sent>;

auto governed(const Address &address, Whole at) -> TIMELINE::Governed;

constexpr Whole BLOCK = 512;

auto order() -> const Vector<Whole> &;

auto ordered() -> Flag;

struct Walk {
  Whole start = NONE;
  Whole cut = 0;
  Whole from = 0;
};

auto walk(Whole start, Whole frames, const Span &cycle) -> Walk;

auto walk(Whole start, Whole frames) -> Walk;

auto after(const Walk &walk, Whole frames) -> Whole;

auto block(const Walk &walk, Whole frames) -> Flag;

auto block(Whole start, Whole frames) -> Flag;

void forget();

auto sounded(const String &node)
  -> const Vector<Vector<AUDIO::PLUGIN::Sample>> &;

auto landed(const String &node, Whole in)
  -> const Vector<Vector<AUDIO::PLUGIN::Sample>> &;

auto delivered() -> Whole;

void spool();

void follow();

void settle();

namespace VOICE {

void claim();

void drop();

}  // namespace VOICE

namespace TAP {

struct Aim {
  String node;
  Whole in = 0;
};

struct Tapped {
  Whole at = 0;
  Whole in = 0;
  Vector<Vector<Float>> lanes;
};

void aim(const Vector<Aim> &ins);

auto drained() -> Vector<Tapped>;

}  // namespace TAP

}  // namespace SOUND::RENDER

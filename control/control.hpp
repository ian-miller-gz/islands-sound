// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <cartridge/plugin.hpp>
#include <island/audio.hpp>
#include <island/midi.hpp>

#include "../stock.hpp"

namespace SOUND::CONTROL {

constexpr Whole LOUDEST = 127;
constexpr Whole PITCHES = 128;

constexpr Whole GRAIN = 512;

struct Played {
  Whole pitch = 0;
  Whole at = 0, length = 0;
  Float velocity = 0.0f;
};

struct Turned {
  Address address;
  Whole at = 0;
  Float value = 0.0f;
};

struct Crossed {
  Whole lane = 0;
  Vector<Vector<Float>> lanes;
};

struct Take {
  Vector<Played> notes;
  Vector<Vector<Float>> heard;
  Vector<Crossed> tapped;
  Vector<Turned> turned;
  Whole at = 0;
  Whole frames = 0;
  Float tempo = TEMPO;
};

struct Reel {
  Flag armed = false;
  Flag taking = false;
  Whole at = 0;
  Whole frames = 0;
  Whole notes = 0;
  Whole heard = 0;
  Whole tapped = 0;
  Whole turned = 0;
  Vector<Whole> crossed;
};

void drive(Whole track);
auto driven() -> Whole;

void aim(const String &node);
auto aimed() -> String;

void settle(const Vector<String> &gone);

auto read() -> Whole;
auto read(const MIDI::Message &message) -> Flag;

auto tap(Whole at, Whole lane, const Vector<Vector<Float>> &lanes) -> Whole;

struct Input {
  String device;
  Whole channel = 0;
};

auto inputs() -> Vector<AUDIO::INPUT::Device>;

auto offered(const String &device) -> Flag;

void choose(const String &device);

void choose(Whole channel);

auto chosen() -> Input;

auto hear() -> Whole;
auto hear(const AUDIO::Sample *from, Whole frames) -> Whole;

void turn(const Address &address, Float value);

auto held() -> const Vector<Value> &;

void adopt(const Vector<Value> &values);

constexpr STRING::Hot ARC = "arc";
constexpr STRING::Hot CURVE = "curve";
constexpr STRING::Hot VALUE = "value";
constexpr STRING::Hot MEMORY = "memory";

struct Offer {
  Flag arced = false;
  Float arc = 0.0f;
  Flag governed = false;
  Float curve = 0.0f;
  Float memory = 0.0f;
};

struct Sounded {
  STRING::Hot from = MEMORY;
  Float value = 0.0f;
};

auto sounding(const Address &address, const Offer &offer) -> Sounded;

auto arrived() -> const Vector<AUDIO::PLUGIN::Event> &;
void taken();

void arm(Flag on);
auto reel() -> Reel;

auto tend(Flag rolling, Whole position, Float tempo) -> Flag;

auto landed() -> const Take &;

void close();

}  // namespace SOUND::CONTROL

// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "control.hpp"

namespace SOUND::CONTROL::MAP {

struct Binding {
  Address address;
  Whole number = NONE;
  Whole channel = NONE;
};

struct Turn {
  Address address;
  Float fraction = 0.0f;
};

void arm(const Address &address);

void cancel();

auto listening() -> Address;

void disarm(const Address &address);

auto bound(const Address &address) -> Binding;

auto held() -> const Vector<Binding> &;

void adopt(const Vector<Binding> &bindings);

void settle(const Vector<String> &gone);

auto heard(const MIDI::Message &message) -> Flag;

auto scaled(const AUDIO::PLUGIN::Control &control, Float fraction) -> Float;

auto drained() -> Vector<Turn>;

}  // namespace SOUND::CONTROL::MAP

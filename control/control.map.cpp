// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../plugin.hpp"
#include "control.map.hpp"

namespace {
using namespace SOUND;
using CONTROL::MAP::Binding;

struct Learn {
  Vector<Binding> bindings;
  Address listen;
  Vector<CONTROL::MAP::Turn> turns;
};
Learn learn;

auto same(const Address &one, const Address &two) -> Flag {
  return one.node == two.node && one.parameter == two.parameter;
}

void unbind(const Address &address) {
  std::erase_if(::learn.bindings, [&](const Binding &binding) {
    return ::same(binding.address, address);
  });
}

void bind(const MIDI::Message &message) {
  ::unbind(::learn.listen);
  ::learn.bindings.push_back(
    {.address = ::learn.listen,
     .number = message.first,
     .channel = message.channel});
  ::learn.listen = {};
}

}  // namespace

void SOUND::CONTROL::MAP::arm(const Address &address) {
  ::learn.listen = address;
}

void SOUND::CONTROL::MAP::cancel() { ::learn.listen = {}; }

auto SOUND::CONTROL::MAP::listening() -> Address { return ::learn.listen; }

void SOUND::CONTROL::MAP::disarm(const Address &address) {
  ::unbind(address);
  if (::same(::learn.listen, address)) cancel();
}

auto SOUND::CONTROL::MAP::bound(const Address &address) -> Binding {
  for (const Binding &binding : ::learn.bindings)
    if (::same(binding.address, address)) return binding;
  return {.address = address};
}

auto SOUND::CONTROL::MAP::held() -> const Vector<Binding> & {
  return ::learn.bindings;
}

void SOUND::CONTROL::MAP::adopt(const Vector<Binding> &bindings) {
  ::learn = {.bindings = bindings};
}

void SOUND::CONTROL::MAP::settle(const Vector<String> &gone) {
  for (const String &node : gone) {
    std::erase_if(::learn.bindings, [&](const Binding &binding) {
      return binding.address.node == node;
    });
    if (::learn.listen.node == node) cancel();
  }
}

auto SOUND::CONTROL::MAP::heard(const MIDI::Message &message) -> Flag {
  if (!::learn.listen.node.empty()) {
    ::bind(message);
    return true;
  }
  Flag turned = false;
  for (const Binding &binding : ::learn.bindings)
    if (binding.number == message.first && binding.channel == message.channel) {
      ::learn.turns.push_back(
        {.address = binding.address,
         .fraction = Float(message.second) / Float(LOUDEST)});
      turned = true;
    }
  return turned;
}

auto SOUND::CONTROL::MAP::drained() -> Vector<Turn> {
  Vector<Turn> handed;
  handed.swap(::learn.turns);
  return handed;
}

auto SOUND::CONTROL::MAP::scaled(
  const AUDIO::PLUGIN::Control &control, Float fraction) -> Float {
  return PLUGIN::scaled(control, fraction * PLUGIN::FULL);
}

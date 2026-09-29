// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <cartridge/plugin.hpp>

namespace SOUND::PLUGIN {

constexpr Whole ROOM = 512;

constexpr STRING::Hot NATIVE = "native";
constexpr STRING::Hot VST3 = "vst3";

constexpr STRING::Hot MONO = "mono";
constexpr STRING::Hot POLY = "poly";

struct Offer {
  STRING::Hot name = "";
  STRING::Hot from = NATIVE;
  STRING::Hot type = "";
  STRING::Hot voicing = "";
  const AUDIO::PLUGIN::Plug *surface = nullptr;
  auto (*answer)(
    void *instance, const AUDIO::PLUGIN::Event *events, Whole count,
    AUDIO::PLUGIN::Event *out, Whole room) -> Whole = nullptr;
  void (*listen)(void *instance) = nullptr;
  auto (*bind)(void *instance, const String &device, Whole lane) -> Flag =
                                                                      nullptr;
};

auto offer(const Offer &row) -> Flag;

auto catalog() -> const Vector<Offer> &;

auto found(const String &name) -> const Offer *;
auto bound(const String &name) -> Flag;

constexpr Float FULL = 127.0f;

auto scaled(const AUDIO::PLUGIN::Control &described, Float drawn) -> Float;

}  // namespace SOUND::PLUGIN

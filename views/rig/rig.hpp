// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <cartridge/interface.hpp>
#include <island/gui/gui.hpp>

namespace SOUND::VIEWS::RIG {

struct Source {
  STRING::Hot name = nullptr;
  auto (*published)() -> AUDIO::PLUGIN::Control = nullptr;
  auto (*read)() -> Float = nullptr;
  void (*write)(Float value) = nullptr;
};

constexpr STRING::Hot PATTERN = "rig";
constexpr STRING::Hot NAMED = "name";
constexpr STRING::Hot STEPPED = "steps";
constexpr STRING::Hot DIALLED = "dial";
constexpr STRING::Hot BOUND = "bound";
constexpr STRING::Hot RESTORED = "restore";
constexpr STRING::Hot HINTED = "restore.hint";

constexpr Float LETTER = 15.0f;
constexpr Float AIR = 10.0f;

auto dressed(GUI::Handle page, const AUDIO::PLUGIN::Control &published)
  -> GUI::Dial;
auto worded(const AUDIO::PLUGIN::Control &published, Float value) -> String;
auto worded(
  const AUDIO::PLUGIN::Control &published, Float value,
  const String &reading) -> String;
auto places(const String &reading) -> Whole;
auto parsed(
  const AUDIO::PLUGIN::Control &published, const String &run,
  Float &value) -> Flag;
auto stepped(const AUDIO::PLUGIN::Control &published, Float value) -> Whole;
auto valued(const AUDIO::PLUGIN::Control &published, Whole step) -> Float;
auto listed(const AUDIO::PLUGIN::Control &published) -> Flag;

auto column(GUI::Handle page, STRING::Hot list, const Vector<String> &names)
  -> Float;
void seat(
  GUI::Handle page, const String &cell, Float column, Float room, Float gap);

auto fitted(GUI::Handle page, const String &dial, const String &whole)
  -> String;

void turn(
  GUI::Handle page, const String &cell, const Source &source, Float column);

}  // namespace SOUND::VIEWS::RIG

// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <cartridge/interface.hpp>
#include <common/fields.hpp>
#include <island/gui/gui.hpp>
#include <island/gui/nga.hpp>

namespace SOUND::VIEWS {

constexpr STRING::Hot ZOOM = "zoom";
constexpr STRING::Hot PAN = "pan";

constexpr STRING::Hot STRIP = "views";
constexpr STRING::Hot BOARD = "board";
constexpr STRING::Hot ROWS = "rows";

auto icon(STRING::Hot view) -> String;

struct Dash {
  Float at = 0.0f, run = 0.0f, deep = 0.0f;
};

using Placed = Float (*)(Whole frames);

struct Overview {
  Vector<Dash> dashes;
  Whole reach = 0;
  Placed placed = nullptr;
};

struct View {
  STRING::Hot name = "";
  Flag panel = false;
  Flag opening = false;

  Flag flat = false;

  Flag split = false;

  GUI::Extent (*least)() = nullptr;

  GUI::Extent (*most)() = nullptr;

  Vector<STRING::Hot> controls;
  void (*seat)() = nullptr;
  void (*run)() = nullptr;

  void (*door)() = nullptr;

  void (*state)(SHELL::Session &) = nullptr;

  Vector<SHELL::Command> words;

  const View *(*faced)() = nullptr;

  Overview (*overview)() = nullptr;

  void (*keep)(FIELDS::Map &face) = nullptr;

  void (*restore)(const FIELDS::Map &face) = nullptr;

  void (*shed)() = nullptr;
};

auto offer(const View &unit) -> Flag;

auto roster() -> const Vector<View> &;

auto found(const String &name) -> const View *;

auto document() -> GUI::Handle;

auto spot() -> GUI::Position;

auto open(STRING::Hot assets) -> Status;

auto standing() -> STRING::Hot;
auto choose(const String &name) -> Flag;

void show(const View &unit, Flag on);

void shed(const View &unit);

auto raised(const String &name) -> Flag;
auto raise(const String &name, Flag on) -> Flag;

auto owns(const String &name, const String &control) -> Flag;

auto cursor(const String &name) -> Whole;

auto named(const String &node) -> String;

auto device(const String &node) -> String;

auto named(const String &plugin, Whole parameter) -> String;

auto joined(Whole track, Whole lane, const Vector<Whole> &rows) -> Whole;
auto parameters(const String &plugin) -> Whole;

auto split() -> Float;

auto zoom(Float factor) -> Flag;
auto zoom(Float across, Float down) -> Flag;
auto pan(Float x, Float y) -> Flag;

void wheeled(const GUI::NGA::Wheel &turn);

auto journaled(const INPUT::KEYS::Event &key) -> Flag;

auto reborn(Whole &born) -> Flag;

struct Pool {
  Whole stood = 0;
  Whole lit = 0;
  Float seen = 0.0f;
  Whole born = 0;
};

using Named = String (*)(Whole at);

auto sized(Pool &pool, Whole wanted, Float seen) -> Whole;

void built(Pool &pool, Whole count);

void light(Pool &pool, Named named, Whole count);

auto reborn(Pool &pool) -> Flag;

constexpr Float EASE = 0.34f;
constexpr Float SETTLE = 0.5f;
auto eased(Float held, Float wanted) -> Float;

constexpr Float TRIM = 0.07f;

void glide();
auto clock() -> Whole;

auto chasing() -> Flag;
void chasing(Flag on);

void wandered();

auto chased(Float pan, Float at, Float seen) -> Float;

auto seen() -> Flag;

auto elsewhere(STRING::Hot door, STRING::Hot panel) -> Flag;

auto kept() -> FIELDS::Map;

void keep(const FIELDS::Map &face);

auto owned() -> Flag;
void owned(Flag on);

void shaded(GUI::Theme shade);

void settings(SHELL::Session &session);

namespace WIRED {

auto steering() -> String;

auto among(const String &plugin) -> String;

}  // namespace WIRED

namespace MENU {

void raise(const Vector<String> &rows, Float x, Float y, STRING::Hot whose);

void raise(const Vector<String> &rows, STRING::Hot whose, STRING::Hot door);

void raise(
  const Vector<String> &rows, Float x, Float y, STRING::Hot whose, Whole row);

void lower();

auto standing() -> Flag;

auto taken(STRING::Hot whose) -> Whole;

void frame();

}  // namespace MENU

void frame();

void close();

}  // namespace SOUND::VIEWS

// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../graph.hpp"
#include "../views.hpp"

namespace SOUND::VIEWS::WIRED {

constexpr STRING::Hot IN = "in";
constexpr STRING::Hot OUT = "out";

constexpr STRING::Hot QUIET = "quiet";

constexpr STRING::Hot BYPASS = "bypass";

constexpr STRING::Hot FACE = "face";

constexpr STRING::Hot NAME = "name";

constexpr STRING::Hot DEVICE = "device";

constexpr Float ADVANCE = 0.6f;

constexpr Float WIDE = 190.0f;
constexpr Float CELL = 20.0f;

constexpr STRING::Hot NOTICE = "graph.notice";

constexpr STRING::Hot RING = "graph.grasp";

auto board() -> String;

auto socket(const String &box, STRING::Hot side, Whole port) -> String;

auto knob(const String &box, Whole out) -> String;

void label(
  const String &box, const String &socket, const GUI::Position &at,
  const GUI::Extent &size, Float letter, const Port &worn);

auto worded(const Port &worn) -> String;

auto cropped(const String &word, Float wide, Float letter) -> String;

void title(
  const String &box, const GUI::Position &at, const GUI::Extent &band,
  Float letter);

void subtitle(
  const String &box, const GUI::Position &at, const GUI::Extent &band,
  Float letter);

auto attended() -> String;

auto steered() -> Whole;

struct Seen {
  Flag stands = false;
  Whole branch = NONE, depth = NONE;
};

auto picture() -> Vector<Seen>;

void follow(Flag again);

auto hands() -> Flag;

void carried(GUI::Handle page);

auto pinned() -> Flag;

void acts();

void door();

void doors();

constexpr STRING::Hot BUSES = "buses";
constexpr STRING::Hot TRACKS = "tracks";

constexpr STRING::Hot SOURCES = "in";
constexpr STRING::Hot SINKS = "out";
constexpr STRING::Hot NOTED = "midi";

constexpr STRING::Hot DOORS = "graph.palette.door";

void menu();

auto unseat(const String &node) -> Flag;

auto dismiss(const String &root) -> Flag;

void take(const String &plug);

void surface(const String &plug, const String &device, Whole lane);

void send(const String &root);

void browse();

void raise(Flag on);

void raise(const String &filing);

auto browsing() -> Flag;
auto filed() -> String;

void say(SHELL::Session &session);

void grid();

auto window() -> GUI::Extent;

auto deep(const Node &node) -> Float;

auto placed(Whole index, const Vector<Seen> &seen, const GUI::Extent &view)
  -> GUI::Position;

void aim();

void draw();

void bound();

auto least() -> GUI::Extent;

}  // namespace SOUND::VIEWS::WIRED

namespace SOUND::VIEWS::WIRED::PALETTE {

struct Door {
  String word, filing;
};

auto doors() -> Vector<Door>;
auto sentence(const Door &door) -> String;

}  // namespace SOUND::VIEWS::WIRED::PALETTE

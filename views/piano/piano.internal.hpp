// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui/carry.hpp>
#include <island/gui/ladder.hpp>
#include <island/gui/stroke.hpp>

#include "../../history.hpp"
#include "../../score.hpp"
#include "../views.marks.hpp"
#include "piano.face.hpp"

namespace SOUND::VIEWS::ROLL {

constexpr Whole GRAIN = 24;
constexpr Whole HIGHEST = 127;
constexpr Whole SEMITONES = 12;
constexpr Float STEP = 16.0f;
constexpr Float TALL = 14.0f;
constexpr Float LEAST = 12.0f;
constexpr Float DEEP = Float(HIGHEST + 1) * STEP;

constexpr auto across(Whole pulses) -> Float {
  return Float(pulses) / Float(GRAIN);
}
constexpr auto lane(Whole pitch) -> Float {
  return pitch < HIGHEST ? Float(HIGHEST - pitch) * STEP : 0.0f;
}

constexpr Float HALF = 0.5f;
constexpr auto across(Float place) -> Whole {
  return place <= 0.0f ? 0 : Whole(place + HALF) * GRAIN;
}
constexpr auto lane(Float place) -> Whole {
  const Whole row = place <= 0.0f ? 0 : Whole(place / STEP + HALF);
  return row < HIGHEST ? HIGHEST - row : 0;
}

constexpr auto laid(Whole pulses, Whole snap) -> Whole {
  return snap == 0 ? pulses : pulses - pulses % snap;
}

auto board() -> String;
auto scaled() -> Float;

auto window() -> GUI::Extent;

constexpr Whole REACH = Whole(Float(10 * 3600) * TEMPO / 60.0f) * PULSES;
auto least() -> GUI::Extent;

void bound();

void park();

void chase();

void keys();
void keys(SHELL::Session &session);

struct Keyed {
  Key key;
  Float from = 0.0f, to = 0.0f;
};

auto keyed(Float reach) -> Vector<Keyed>;

auto dressed(Whole pitch, const Key &key) -> String;
auto named(Whole pitch) -> String;

auto rekeyed(Vector<Key> &held) -> Flag;

auto sectioned(STRING::Hot stem, Whole section, Whole pitch) -> String;

auto marked() -> Whole;

void doors();
void doors(SHELL::Session &session);

void changes();
void changes(SHELL::Session &session);

auto degreed(Whole pitch, Whole at) -> Whole;

constexpr Float FULL = Float(HIGHEST);

struct Bench {
  Whole snap = 0, length = 0;
  Float loud = 0.0f;
};
auto bench() -> Bench;

void dials();
void dials(SHELL::Session &session);

void rail();
void rail(SHELL::Session &session);

constexpr Float RAILED = 52.0f;

void easel();
void easel(SHELL::Session &session);

void begun(GUI::Handle page, const GUI::SAC::STROKE::Cell &cell);
void painted(GUI::Handle page, const Vector<GUI::SAC::STROKE::Cell> &cells);
auto ended(GUI::Handle page, const GUI::SAC::STROKE::Cell &cell, Flag dragged)
  -> Flag;

auto grained() -> Whole;

struct Aim {
  Whole stock = NONE, from = 0, opens = 0;
};
auto aimed() -> Aim;
auto aimed(Whole at) -> Aim;
auto aimed(Whole track, Whole at) -> Aim;
void aimed(SHELL::Session &session);

auto claimed(Whole pulse) -> Aim;
auto claimed(Whole track, Whole pulse) -> Aim;

auto laned(Whole track) -> Whole;

void seams();
void seams(SHELL::Session &session);

namespace SEAMS {
constexpr Float MARK = 32.0f;
void built(
  GUI::Handle page, const String &bed, Whole count, Whole &stood,
  STRING::Hot kind, STRING::Hot dress);
void lined(GUI::Handle page, const Vector<Whole> &flush, Whole &stood);
constexpr STRING::Hot EDGE = "edge";
void bared(GUI::Handle page, const String &bed, Whole &stood);
}  // namespace SEAMS

struct Span {
  Whole stock = NONE, opens = 0, closes = 0;
  String name;
};
auto spans() -> Vector<Span>;

void range();
void range(SHELL::Session &session);

constexpr Float LIT = 0.5f;
constexpr Float CREST = 1.0f;

auto opened(Whole frames) -> Whole;

constexpr auto inside(const Aim &aim, Whole pulse) -> Whole {
  return pulse > aim.opens ? aim.from + (pulse - aim.opens) : aim.from;
}

constexpr STRING::Hot NOTICE = "piano.notice";

constexpr Float SEATED = 1.0f;
constexpr Float HELD = 3.0f;

constexpr Float LOOSE = 2.0f;

struct Pip {
  Whole stock = NONE, index = NONE, pitch = 0, at = 0, length = 0;
  Whole track = NONE;
};

struct Loose {
  Whole pitch = 0, at = 0, length = 0;
  Float velocity = 0.0f;
  Whole track = NONE;
};

auto chosen(GUI::Handle page) -> Vector<Whole>;

void walk(GUI::Handle page);

void copy(GUI::Handle page);
void paste(GUI::Handle page);

void grasped(GUI::Handle page);

auto loose() -> const Vector<Loose> &;
void loosed(Vector<Pip> &wanted);

struct Fallen {
  Whole slot = NONE, at = 0, pitch = 0;
};

auto land(GUI::Handle page, const Vector<Fallen> &fallen) -> Vector<Pip>;

void stretched(Whole slot, Whole length);
void discard(const Vector<Whole> &slots);

auto plates() -> const Vector<Pip> &;
auto wide(Whole length) -> Float;

void hands();

void follow(const Vector<Pip> &landed);

auto pluck(GUI::Handle page, Whole row) -> Flag;
void plucked(GUI::Handle page);

void keyed();

auto watched() -> GUI::SAC::CARRY::Watch;

void steadied(GUI::Handle page, Vector<Pip> &wanted);

void grid();
void grid(SHELL::Session &session);

void head();
void head(SHELL::Session &session);

auto strip() -> String;

void ruler();
void ruler(SHELL::Session &session);

auto marks() -> Vector<GUI::SAC::LADDER::Mark>;

auto sections() -> Vector<TRANSPORT::Section>;
auto opening() -> TRANSPORT::Section;

void cycle(
  const GUI::SAC::LADDER::Rung &step,
  const Vector<GUI::SAC::LADDER::Mark> &dressed);

void cycle(SHELL::Session &session);

auto overview() -> VIEWS::Overview;

void shed();

namespace SHED {

void plates();
void hands();
void keys();
void grid();
void head();
void rail();
void range();
void ruler();
void cycle();
void changes();
void seams();

}  // namespace SHED

}  // namespace SOUND::VIEWS::ROLL

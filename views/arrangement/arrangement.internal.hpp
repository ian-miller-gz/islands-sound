// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui/ladder.hpp>
#include <island/gui/stroke.hpp>

#include "../../score.hpp"
#include "../views.hpp"
#include "../views.marks.hpp"

namespace SOUND::VIEWS::ARRANGEMENT {

constexpr Whole GRAIN = 480;
constexpr Float MARGIN = 22.0f;
constexpr Float INLAY = 4.0f;

constexpr Float GUTTER = 120.0f;

constexpr Float INSET = 6.0f;
constexpr Float PAD = 2.0f;
constexpr Float NAMED = GUTTER - INSET * 2.0f;

constexpr Float TITLE = 20.0f;

constexpr Float CABLE = 10.0f;
constexpr Float SHUT = 44.0f;
constexpr Float OPEN = 96.0f;
constexpr Float GAP = 6.0f;
constexpr Float CURB = 3.0f;

constexpr Float SHELF = 44.0f;

constexpr Float RAISED = 1.0f;

constexpr Float HEADER = 4.0f;

constexpr Float CREST = 5.0f;

constexpr Float BLADE = 3.0f;

auto gutter(Float across) -> Flag;

constexpr Whole TEN_HOURS = Whole(10) * 3600 * RATE;

constexpr Float HAIR = 1.0f;

constexpr Float GRIP = 6.0f;

auto board() -> String;

auto scaled() -> Float;

auto strip() -> String;

constexpr auto across(Whole frames) -> Float {
  return Float(frames) / Float(GRAIN);
}

auto window() -> Float;

auto least() -> GUI::Extent;

auto deep() -> Float;

auto rail(Whole band) -> Float;

auto depth(Whole band) -> Float;

auto inlay(Whole band) -> Float;
auto inlaid(Whole band) -> Float;

auto banded(Whole track, Whole lane) -> Whole;

auto plated(Whole kind) -> String;

auto shelf(Whole band) -> Float;

auto shelved(Float down) -> Flag;

void outtakes();
void outtakes(SHELL::Session &session);

void cover();
void cover(SHELL::Session &session);

constexpr Float CROWD = 44.0f;

auto beat() -> Whole;
auto rung() -> GUI::SAC::LADDER::Rung;
auto marks() -> Vector<GUI::SAC::LADDER::Mark>;

auto sections() -> Vector<TRANSPORT::Section>;
auto opening() -> TRANSPORT::Section;

auto bar() -> Whole;

void ruler();
void ruler(SHELL::Session &session);

void bands();
void bands(SHELL::Session &session);

struct Ribbon {
  Float top = 0.0f, bottom = 0.0f;
  String name;
};

auto ribbons() -> Vector<Ribbon>;
void names();

auto ribboned(Float down) -> Whole;

void choose();

struct Row {
  Whole track = NONE, lane = 0, band = 0;
};
auto row(Float down) -> Row;

struct Draft {
  Whole track = NONE, lane = 0, band = 0, kind = 0, at = 0, frames = 0;
};
auto drafting() -> Draft;

void ghost();

void draft();

void menu();

auto lift(Whole track, Whole lane, Whole placement) -> Flag;

auto lifted(const Vector<Whole> &rows) -> Flag;

auto drawing(const GUI::SAC::STROKE::Gesture &gesture) -> Flag;
void easel();
void easel(SHELL::Session &session);

auto chosen() -> Vector<Whole>;

void chosen(const Vector<Whole> &rows);

auto mates(Whole row) -> Vector<Whole>;

auto rowed(Whole track, Whole lane, Whole placement) -> Whole;

void walk();

void marquee();

void keys();

struct Box {
  Whole track = 0, lane = 0, band = 0, kind = 0, placement = 0, stock = NONE,
        at = 0, from = 0, frames = 0;
};

auto boxes() -> const Vector<Box> &;

struct Pip {
  Float at = 0.0f, run = 0.0f, down = 0.0f, deep = 0.0f;
};

auto pips(const Box &box) -> Vector<Pip>;

void sketch();
void sketch(SHELL::Session &session);

void notches();
void notches(SHELL::Session &session);

auto marks(const Box &box) -> Vector<Whole>;

auto stocked(const Box &box, Whole frames) -> Whole;

auto boxed(Float across, Float down) -> Whole;

auto drawn(Whole row) -> Box;

void ends();

auto sketched(Whole band) -> Flag;

auto gripped(Whole frames) -> Flag;

auto snapped() -> Flag;
void snap();
void snap(SHELL::Session &session);

void grips();

void head();

void cap();

void cycle(
  const GUI::SAC::LADDER::Rung &step,
  const Vector<GUI::SAC::LADDER::Mark> &dressed);

void cycle(SHELL::Session &session);

auto map() -> String;

constexpr Float RATIO = 0.1f;

auto stride() -> Float;

void minimap();
void minimap(SHELL::Session &session);

void slider();

void chase();

void shed();

namespace SHED {

void sketch();
void notches();

}  // namespace SHED

}  // namespace SOUND::VIEWS::ARRANGEMENT

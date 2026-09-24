// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui/carry.hpp>
#include <island/gui/ladder.hpp>
#include <island/gui/stroke.hpp>

#include "../../history.hpp"
#include "../../score.hpp"
#include "../../shape.hpp"
#include "../views.marks.hpp"
#include "signals.face.hpp"

namespace SOUND::VIEWS::SIGNALS {

constexpr Whole GRAIN = 480;
constexpr Whole FULL = 127;
constexpr Float STEP = 4.0f;
constexpr Float DEEP = Float(FULL + 1) * STEP;

constexpr Whole REACH = Whole(10) * 3600 * RATE;

constexpr Float PLATE = 8.0f;
constexpr Float TREAD = 2.0f;
constexpr Float HAIR = 2.0f;
constexpr Float LAID = 0.5f;
constexpr Float SEATED = 1.0f;
constexpr Float BLADE = 2.0f;
constexpr Float NORTH = 0.0f;
constexpr Float ORIGIN = 0.0f;
constexpr Float HALF = 0.5f;
constexpr Whole COARSER = 2;

constexpr auto across(Whole frames) -> Float {
  return Float(frames) / Float(GRAIN);
}

constexpr auto down(Float value) -> Float {
  return (Float(FULL) - value) * STEP;
}

constexpr auto valued(Integer row) -> Float {
  const Integer top = Integer(FULL);
  if (row <= 0) return Float(top);
  return row < top ? Float(top - row) : 0.0f;
}

auto board() -> String;

auto scaled() -> Float;

auto sunk() -> Float;

auto strip() -> String;

auto column() -> String;

auto window() -> Float;

auto least() -> GUI::Extent;

void bound();
void chase();

struct Point {
  Whole stock = NONE, index = NONE, at = 0, until = 0;
  Float value = 0.0f;
  Whole shape = SHAPE::HOLD;
};

auto points() -> Vector<Point>;

struct Row {
  Whole at = NONE;
  Float value = 0.0f;
  Whole shape = SHAPE::HOLD;
};
auto row(Whole stock, Whole index) -> Row;

auto covered(Whole stock, Whole at) -> Whole;

auto clear(Whole stock, Whole opens, Whole closes) -> Whole;

auto write(Whole stock, const Row &laid) -> Flag;

void plates();
void plates(SHELL::Session &session);

auto plated() -> Whole;

auto wide() -> Float;

auto deep() -> Float;

void segments();

auto dressed(STRING::Hot stem) -> String;

auto shown() -> const Vector<Point> &;

auto overview() -> VIEWS::Overview;

struct Aim {
  Whole stock = NONE, from = 0, opens = 0;
};
auto attended() -> Whole;

auto aimed() -> Aim;
auto aimed(Whole at) -> Aim;
void aimed(SHELL::Session &session);

constexpr auto inside(const Aim &aim, Whole at) -> Whole {
  return at > aim.opens ? aim.from + (at - aim.opens) : aim.from;
}

struct Span {
  Whole stock = NONE, opens = 0, closes = 0;
};
auto spans() -> Vector<Span>;

auto seen() -> Span;

void range();
void range(SHELL::Session &session);

void bench();
void bench(SHELL::Session &session);

auto grain() -> Whole;

void easel();
void easel(SHELL::Session &session);

void begun();
void drawn(const Vector<GUI::SAC::STROKE::Cell> &cells);
auto landed() -> Whole;

auto stamped(Integer across) -> Whole;

void preview(const GUI::SAC::STROKE::Gesture &gesture);
void preview(SHELL::Session &session);

struct Shape {
  STRING::Hot id = "";
  Whole shape = SHAPE::HOLD;
};

auto shapes() -> const Vector<Shape> &;

void spanned(
  const GUI::SAC::STROKE::Cell &from, const GUI::SAC::STROKE::Cell &to,
  Whole shape);

void thinned(Vector<Point> &drawn);

void drafted(Vector<Point> &wanted);

void hands();

auto watched() -> GUI::SAC::CARRY::Watch;

void land(GUI::Handle page, const Point &held, Whole row);

void followed(GUI::Handle page, const Vector<Point> &wanted);

void steadied(Vector<Point> &wanted);

auto chosen() -> Vector<Whole>;

auto erase(GUI::Handle page, const Vector<Whole> &rows) -> Whole;

void keyed();

void menu();

void pager();
void pager(SHELL::Session &session);

auto number() -> Whole;

void number(Whole paged);

auto named() -> String;

auto unit() -> String;

auto spoken(Float value) -> String;

auto delivered(Float value) -> Float;

auto laned() -> Whole;

struct Word {
  Whole number = NONE;
  String name;
};

auto worded() -> Vector<Word>;

void numbers();
void numbers(SHELL::Session &session);

struct Offer {
  String name;
  Whole words = NONE;
};

auto reached() -> Vector<Offer>;
auto offered() -> Vector<Offer>;

auto seats(STRING::Hot id, Whole rows) -> Vector<Whole>;

void listing(STRING::Hot id, const Vector<Offer> &rows);

void mapping();
void mapping(SHELL::Session &session);

auto mapped() -> String;

auto beat() -> Whole;
auto bar() -> Whole;

auto sections() -> Vector<TRANSPORT::Section>;
auto opening() -> TRANSPORT::Section;

void grid();
void grid(SHELL::Session &session);
void head();
void head(SHELL::Session &session);
void ruler();
void ruler(SHELL::Session &session);

void rail();
void rail(SHELL::Session &session);

auto rung() -> Whole;
auto marks() -> Vector<GUI::SAC::LADDER::Mark>;

void shed();

namespace SHED {

void plates();
void segments();
void grid();
void head();
void rail();
void range();
void ruler();

}  // namespace SHED

}  // namespace SOUND::VIEWS::SIGNALS

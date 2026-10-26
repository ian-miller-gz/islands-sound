// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../control.hpp"
#include "../views.hpp"

namespace SOUND::VIEWS::RACK {

struct Row {
  String node;
  Whole parameter = 0;

  auto operator==(const Row &other) const -> Flag = default;
};

auto aimed() -> String;
auto rows() -> Vector<Row>;
auto counted(const String &node) -> Whole;

auto head(const Row &row) -> Flag;
auto headed(const Row &row) -> String;
auto titled(const Row &row) -> String;
auto named(const Row &row) -> String;
auto stated(const Row &row) -> String;

void menu(GUI::Handle page);

void turn(GUI::Handle page, const String &row, const Row &held);

void typed(GUI::Handle page, const String &cell, const Row &row);

auto held(const Row &row) -> Float;
auto shown(const Row &row) -> String;

auto described(const Row &row) -> AUDIO::PLUGIN::Control;

void moved(const Row &row, Float value);

auto stepped(const AUDIO::PLUGIN::Control &published, Float value) -> Whole;
auto valued(const AUDIO::PLUGIN::Control &published, Whole step) -> Float;

auto listed(const Row &row) -> Flag;
auto listed(const AUDIO::PLUGIN::Control &published) -> Flag;

constexpr STRING::Hot BARRED = "bar";
constexpr STRING::Hot MARKED = "mark";
constexpr STRING::Hot NAMED = "name";
constexpr STRING::Hot HEADED = "head";
constexpr STRING::Hot STEPPED = "steps";

auto column(GUI::Handle page, const Vector<Row> &wanted) -> Float;
auto cut(GUI::Handle page, const Row &row, Float column) -> String;

void seat(GUI::Handle page, const String &cell, Float column);
void title(GUI::Handle page, const String &cell, const Row &row, Float column);
void show(GUI::Handle page, const String &cell, const Row &row, Float column);

auto centred(const AUDIO::PLUGIN::Control &published) -> Flag;

void mark(
  GUI::Handle page, const String &cell,
  const AUDIO::PLUGIN::Control &published);

void door(GUI::Handle page, const String &cell, const Row &row);

constexpr STRING::Hot PLATE = "params.choices";
constexpr STRING::Hot WORDS = "params.choices.rows";

void plate(GUI::Handle page, const Vector<String> &run);

void raise(GUI::Handle page, const String &cell, const Row &row);

void seated(GUI::Handle page, const String &cell, const Row &row);

auto addressed(const Row &row) -> Address;

auto listening(const Row &row) -> Flag;

void bound(GUI::Handle page, const String &cell, const Row &row);
void bound(SHELL::Session &session);

void cancel();

void keep(FIELDS::Map &face);
void restore(const FIELDS::Map &face);

auto words() -> Vector<SHELL::Command>;

void chooser();
void chooser(SHELL::Session &session);

auto choosing() -> Flag;

}  // namespace SOUND::VIEWS::RACK

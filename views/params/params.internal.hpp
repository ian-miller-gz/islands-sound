// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../views.hpp"

namespace SOUND::VIEWS::RACK {

struct Row {
  String node;
  Whole parameter = 0;

  auto operator==(const Row &other) const -> Flag = default;
};

void turn(GUI::Handle page, const String &row, const Row &held);

auto held(const Row &row) -> Float;
auto shown(const Row &row) -> String;

auto described(const Row &row) -> AUDIO::PLUGIN::Control;

void moved(const Row &row, Float value);

auto stepped(const AUDIO::PLUGIN::Control &published, Float value) -> Whole;
auto valued(const AUDIO::PLUGIN::Control &published, Whole step) -> Float;

auto listed(const Row &row) -> Flag;

void door(GUI::Handle page, const String &cell, const Row &row);

constexpr STRING::Hot PLATE = "params.choices";
constexpr STRING::Hot WORDS = "params.choices.rows";

void plate(GUI::Handle page, const Vector<String> &run);

void raise(GUI::Handle page, const String &cell, const Row &row);

void seated(GUI::Handle page, const String &cell, const Row &row);

void chooser();
void chooser(SHELL::Session &session);

auto choosing() -> Flag;

}  // namespace SOUND::VIEWS::RACK

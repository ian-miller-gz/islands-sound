// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui.hpp>

#include "../commands.hpp"
#include "views.hpp"

namespace SOUND::VIEWS::CHANGES {

constexpr Float DIALLED = 44.0f;
constexpr Float TYPED = 32.0f;
constexpr Float KEYED = 72.0f;
constexpr Float INSET = 3.0f;
constexpr Float UPPER = 16.0f;
constexpr Float LOWER = 28.0f;
constexpr Float HANDLE = 6.0f;

struct Tape {
  String strip;
  Float head = 0.0f;
  auto (*placed)(Whole frames) -> Float = nullptr;
  auto (*pulsed)(Float place) -> Whole = nullptr;
};

struct Mark {
  String id;
  Whole at = 0;
  Float east = 0.0f;
  auto (*erase)(Whole at) -> Flag = nullptr;
  auto (*move)(Whole from, Whole to) -> Flag = nullptr;
};

void lay(const Tape &tape);
void shed(const Tape &tape);
void state(SHELL::Session &session, const Tape &tape);

auto beside(const Tempo &row) -> Float;
auto beside(const Metre &row) -> Float;
auto beside(const Key &row) -> Float;

auto worded(const Tempo &row) -> String;
auto worded(const Metre &row) -> String;
auto worded(const Key &row) -> String;

void dressed(GUI::Handle page, const String &id, const Tempo &row);
void dressed(GUI::Handle page, const String &id, const Metre &row);
void dressed(GUI::Handle page, const String &id, const Key &row);

auto taken(GUI::Handle page, const String &id, const Tempo &row) -> Flag;
auto taken(GUI::Handle page, const String &id, const Metre &row) -> Flag;
auto taken(GUI::Handle page, const String &id, const Key &row) -> Flag;

auto paced(Whole from, Whole to) -> Flag;
auto metred(Whole from, Whole to) -> Flag;
auto keyed(Whole from, Whole to) -> Flag;

void seated(GUI::Handle page, const String &strip);

void settled();

void asked(const Vector<Mark> &marks);

void handled(const Tape &tape, const Vector<Mark> &marks);

}  // namespace SOUND::VIEWS::CHANGES

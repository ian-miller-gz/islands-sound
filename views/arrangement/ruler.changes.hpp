// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui.hpp>

#include "../../commands.hpp"

namespace SOUND::VIEWS::ARRANGEMENT {

void changes();

namespace CHANGES {

constexpr Float DIALLED = 44.0f;
constexpr Float TYPED = 32.0f;
constexpr Float INSET = 3.0f;

struct Mark {
  String id;
  Whole at = 0;
  auto (*erase)(Whole at) -> Flag = nullptr;
};

void asked(const Vector<Mark> &marks);

auto beside(const Tempo &row) -> Float;
auto beside(const Metre &row) -> Float;

auto worded(const Tempo &row) -> String;
auto worded(const Metre &row) -> String;

void dressed(GUI::Handle page, const String &id, const Tempo &row);
void dressed(GUI::Handle page, const String &id, const Metre &row);

auto taken(GUI::Handle page, const String &id, const Tempo &row) -> Flag;
auto taken(GUI::Handle page, const String &id, const Metre &row) -> Flag;

void seated(GUI::Handle page, const String &stem);

void settled();

}  // namespace CHANGES

}  // namespace SOUND::VIEWS::ARRANGEMENT

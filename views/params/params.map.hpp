// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../control.hpp"
#include "../views.hpp"

namespace SOUND::VIEWS::RACK {

constexpr STRING::Hot MAPPED = "params.map";

void mapped(GUI::Handle page);
void mapped(SHELL::Session &session);

void type(const Address &address);

void map(
  GUI::Handle page, STRING::Hot list,
  const Vector<CONTROL::MAP::Binding> &rows);

}  // namespace SOUND::VIEWS::RACK

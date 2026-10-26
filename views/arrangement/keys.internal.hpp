// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../views.hpp"

namespace SOUND::VIEWS::ARRANGEMENT {

auto chosen(GUI::Handle page) -> Whole;

void copy(GUI::Handle page);

void paste(GUI::Handle page);

void divide(GUI::Handle page);

void rename(GUI::Handle page);

void rename(GUI::Handle page, Whole row);

void renaming(GUI::Handle page);

}  // namespace SOUND::VIEWS::ARRANGEMENT

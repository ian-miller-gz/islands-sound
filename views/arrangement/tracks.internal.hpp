// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../views.hpp"

namespace SOUND::VIEWS::TRACKS {

constexpr STRING::Hot NAME = "tracks";
constexpr STRING::Hot ROWS = "tracks.rows";

void edit(Whole track);

void editing();
void editing(SHELL::Session &session);

}  // namespace SOUND::VIEWS::TRACKS

// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../views.hpp"

namespace SOUND::VIEWS::RACK {

void keep(FIELDS::Map &face);
void restore(const FIELDS::Map &face);
void skipped(SHELL::Session &session);

}  // namespace SOUND::VIEWS::RACK

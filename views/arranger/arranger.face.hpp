// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../views.hpp"

namespace SOUND::VIEWS::ARRANGER {

auto editable(Whole kind) -> Flag;

auto edit(Whole track, Whole lane) -> Flag;

auto words() -> Vector<SHELL::Command>;

}  // namespace SOUND::VIEWS::ARRANGER

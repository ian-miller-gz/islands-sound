// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <cartridge/interface.hpp>

namespace SOUND::COMMANDS {

auto offer(const Vector<SHELL::Command> &rows) -> Flag;

auto offered() -> Vector<SHELL::Command>;

auto later() -> Vector<SHELL::Command>;

}  // namespace SOUND::COMMANDS

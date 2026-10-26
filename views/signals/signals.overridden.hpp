// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "signals.internal.hpp"

namespace SOUND::VIEWS::SIGNALS {

auto target() -> Address;
auto overridden() -> Flag;
auto worn(STRING::Hot stem) -> String;
auto sounded() -> Float;
auto reading(Float value) -> String;

void read(Flag on);

void restore();
void restore(SHELL::Session &session);

namespace SHED {

void restore();

}  // namespace SHED

}  // namespace SOUND::VIEWS::SIGNALS

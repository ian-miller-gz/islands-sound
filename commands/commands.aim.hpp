// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../history.hpp"

namespace SOUND::HISTORY::WORD {

constexpr STRING::Hot AIM = "aim";

}  // namespace SOUND::HISTORY::WORD

namespace SOUND::COMMANDS {

namespace CHANGE {

auto aimed(const Change &change) -> String;
auto aimed(const Change &change, const String &value) -> Flag;

}  // namespace CHANGE

auto aimed(Whole stock, const Address &address) -> Flag;

}  // namespace SOUND::COMMANDS

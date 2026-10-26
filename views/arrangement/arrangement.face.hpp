// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../views.hpp"

namespace SOUND::VIEWS::ARRANGEMENT {

auto face() -> const View &;

namespace GET {

auto opened() -> Flag;

}  // namespace GET

}  // namespace SOUND::VIEWS::ARRANGEMENT

// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "views.hpp"

namespace SOUND::VIEWS::LETTERS {

constexpr Float ADVANCE = 0.6f;

auto cropped(const String &word, Float wide, Float letter) -> String;

}  // namespace SOUND::VIEWS::LETTERS

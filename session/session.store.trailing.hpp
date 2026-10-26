// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <format>
#include <sstream>

#include <common.hpp>

namespace SOUND::SESSION {

template <typename Field>
void trailing(std::istringstream &line, Field &field) {
  Field token = field;
  if (line >> token) field = token;
}

inline auto bent(Float bend) -> String {
  return bend == 0.0f ? String() : std::format(" {}", bend);
}

}  // namespace SOUND::SESSION

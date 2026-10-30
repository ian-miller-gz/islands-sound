// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../graph.hpp"
#include "views.internal.hpp"
#include "views.split.internal.hpp"

namespace {

constexpr STRING::Hot FOCUS = "focus";
constexpr STRING::Hot HELD = "bar held";
constexpr STRING::Hot HANDLE = "handle {}";

}  // namespace

void SOUND::VIEWS::SPLIT::state(SHELL::Session &session) {
  const Whole at = lower();
  if (at == NONE) return;
  session.print(
    std::format("split {} {} {}", height(), LOWER, roster()[at].name));
  session.print(
    std::format("{} {} {}", ::FOCUS, REGIONS[focused()], aimed()->name));
  if (held()) session.print(::HELD);
  session.print(std::format(::HANDLE, GET::handle()));
}

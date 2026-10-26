// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../control.hpp"
#include "params.internal.hpp"
#include "params.kept.hpp"
#include "params.map.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot CC = "cc";
constexpr STRING::Hot MUTED = "muted";
constexpr STRING::Hot LISTENING = "listening";
constexpr STRING::Hot MENU = "menu";
constexpr STRING::Hot NOWHERE = "";

}  // namespace

auto SOUND::VIEWS::RACK::addressed(const Row &row) -> Address {
  return {.node = row.node, .parameter = row.parameter};
}

auto SOUND::VIEWS::RACK::listening(const Row &row) -> Flag {
  const Address listen = CONTROL::MAP::listening();
  return !listen.node.empty() && listen.node == row.node &&
         listen.parameter == row.parameter;
}

void SOUND::VIEWS::RACK::bound(
  GUI::Handle page, const String &cell, const Row &row) {
  const String id = cell + "." + BOUND;
  const Whole number = CONTROL::MAP::bound(addressed(row)).number;
  const Flag listened = listening(row);
  GUI::set(page, id.c_str(), GUI::Visibility{listened || number != NONE});
  if (!listened && number == NONE) return;
  GUI::set(page, id.c_str(), GUI::Style{listened ? ::LISTENING : ::MUTED});
  GUI::set(
    page, id.c_str(),
    GUI::Text{listened ? String(::CC) : std::format("{} {}", ::CC, number)});
}

void SOUND::VIEWS::RACK::bound(SHELL::Session &session) {
  for (const CONTROL::MAP::Binding &binding : CONTROL::MAP::held())
    session.print(std::format(
      "bound {} {} cc {} channel {}", binding.address.node,
      binding.address.parameter, binding.number, binding.channel));
  skipped(session);
  const Address listen = CONTROL::MAP::listening();
  if (!listen.node.empty())
    session.print(
      std::format("listening {} {}", listen.node, listen.parameter));
  mapped(session);
}

auto SOUND::VIEWS::RACK::cancel(GUI::Handle page) -> Flag {
  if (CONTROL::MAP::listening().node.empty()) return false;
  const Flag spent = VIEWS::escaped();
  const Flag away =
    VIEWS::elsewhere(::NOWHERE, ::MENU) && VIEWS::elsewhere(::NOWHERE, MAPPED);
  if (spent || CONTROL::MAP::stale() || away) CONTROL::MAP::cancel();
  return spent;
}

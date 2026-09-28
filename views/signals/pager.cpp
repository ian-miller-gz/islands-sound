// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../../kind.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot NUMBER = "lane.number";
constexpr STRING::Hot LABELLED = "lane.number.label";
constexpr STRING::Hot MEANT = "lane.number.name";
constexpr STRING::Hot NOTHING = "none";
constexpr STRING::Hot BETWEEN = " · ";
constexpr Float LEAST = 0.0f;
constexpr Float RESTING = 1.0f;
constexpr Flag LETTERED = false;

Whole paging = KIND::CONTROL;
Flag stood = false;
Whole born = 0;

void shaped(GUI::Handle page) {
  if (VIEWS::reborn(::born)) ::stood = false;
  GUI::set(
    page, ::NUMBER,
    GUI::Dial{
      ::LEAST, Float(VIEWS::SIGNALS::FULL), ::RESTING, VIEWS::SIGNALS::FULL,
      ::LETTERED});
  if (::stood) return;
  GUI::set(page, ::NUMBER, GUI::Value{::RESTING});
  ::stood = true;
}

void pages(GUI::Handle page) {
  const Flag numbered = ::paging == KIND::CONTROL;
  GUI::set(page, ::NUMBER, GUI::Visibility{numbered});
  GUI::set(page, ::LABELLED, GUI::Visibility{numbered});
  GUI::set(page, ::MEANT, GUI::Visibility{numbered});
}

void taken(GUI::Handle page) {
  if (!GUI::GET::committed(page, ::NUMBER)) return;
  GUI::edit(page, "");
  GUI::set(
    page, ::NUMBER,
    GUI::Value{std::strtof(GUI::GET::text(page, ::NUMBER).c_str(), nullptr)});
}

auto dialled(GUI::Handle page) -> Whole {
  return Whole(GUI::GET::value(page, ::NUMBER));
}

auto labelled() -> String {
  const String word = VIEWS::SIGNALS::named(), suffix = VIEWS::SIGNALS::unit();
  if (word.empty() || suffix.empty()) return word;
  return word + ::BETWEEN + suffix;
}

}  // namespace

void SOUND::VIEWS::SIGNALS::pager() {
  const GUI::Handle page = document();
  ::shaped(page);
  ::pages(page);
  ::taken(page);
  if (GUI::GET::editing(page) != ::NUMBER)
    GUI::set(page, ::NUMBER, GUI::Text{std::to_string(::dialled(page))});
  GUI::set(page, ::MEANT, GUI::Text{::labelled()});
}

auto SOUND::VIEWS::SIGNALS::paged() -> Whole { return ::paging; }

void SOUND::VIEWS::SIGNALS::paged(Whole kind) { ::paging = kind; }

auto SOUND::VIEWS::SIGNALS::number() -> Whole {
  if (::paging != KIND::CONTROL) return NONE;
  return ::stood ? ::dialled(document()) : Whole(::RESTING);
}

void SOUND::VIEWS::SIGNALS::number(Whole paged) {
  GUI::set(document(), ::NUMBER, GUI::Value{Float(paged)});
}

void SOUND::VIEWS::SIGNALS::pager(SHELL::Session &session) {
  const String meant = ::labelled(), plugin = mapped();
  const Flag numbered = ::paging == KIND::CONTROL;
  session.print(std::format(
    "pager kind {} number {} cell {} label {} map {}", KIND::spoken(paged()),
    number() == NONE ? String(::NOTHING) : std::to_string(number()),
    numbered ? "shown" : "hidden", meant.empty() ? String(::NOTHING) : meant,
    plugin.empty() ? String(::NOTHING) : plugin));
}

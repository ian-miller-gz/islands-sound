// SPDX-License-Identifier: AGPL-3.0-or-later
#include <charconv>
#include <format>

#include "../../transport.hpp"
#include "ruler.changes.hpp"

namespace {
using namespace SOUND;

constexpr Float NOWHERE = 0.0f;
constexpr char SLASH = '/';

String seat;

auto read(const String &run, Whole &value) -> Flag {
  const char *end = run.data() + run.size();
  const auto [last, fault] = std::from_chars(run.data(), end, value);
  return fault == std::errc() && last == end && value > 0;
}

auto split(const String &run, Metre &row) -> Flag {
  const auto slash = run.find(::SLASH);
  return slash != String::npos && ::read(run.substr(0, slash), row.numerator) &&
         ::read(run.substr(slash + 1), row.denominator);
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::CHANGES::beside(const Metre &row) -> Float {
  for (const Tempo &tempo : TRANSPORT::held().tempos)
    if (tempo.at == row.at) return DIALLED + INSET;
  return ::NOWHERE;
}

auto SOUND::VIEWS::ARRANGEMENT::CHANGES::worded(const Metre &row) -> String {
  return std::format("{}{}{}", row.numerator, ::SLASH, row.denominator);
}

void SOUND::VIEWS::ARRANGEMENT::CHANGES::dressed(
  GUI::Handle, const String &, const Metre &) {}

auto SOUND::VIEWS::ARRANGEMENT::CHANGES::taken(
  GUI::Handle page, const String &id, const Metre &row) -> Flag {
  if (!GUI::GET::committed(page, id.c_str())) return false;
  Metre typed = row;
  if (::split(GUI::GET::text(page, id.c_str()), typed))
    COMMANDS::metred(row.at, typed.numerator, typed.denominator);
  GUI::edit(page, "");
  return true;
}

void SOUND::VIEWS::ARRANGEMENT::CHANGES::seated(
  GUI::Handle page, const String &stem) {
  const String editing = GUI::GET::editing(page);
  if (editing != ::seat && editing.starts_with(stem))
    GUI::set(
      page, editing.c_str(),
      GUI::Caret{Whole(GUI::GET::text(page, editing.c_str()).size()), 0});
  ::seat = editing;
}

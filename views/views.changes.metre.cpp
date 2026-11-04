// SPDX-License-Identifier: AGPL-3.0-or-later
#include <charconv>
#include <format>

#include "../history.hpp"
#include "../transport.hpp"
#include "views.changes.hpp"

namespace {
using namespace SOUND;

constexpr Float NOWHERE = 0.0f;
constexpr char SLASH = '/';

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

auto SOUND::VIEWS::CHANGES::beside(const Metre &) -> Float {
  return ::NOWHERE;
}

auto SOUND::VIEWS::CHANGES::worded(const Metre &row) -> String {
  return std::format("{}{}{}", row.numerator, ::SLASH, row.denominator);
}

void SOUND::VIEWS::CHANGES::dressed(
  GUI::Handle, const String &, const Metre &) {}

auto SOUND::VIEWS::CHANGES::taken(
  GUI::Handle page, const String &id, const Metre &row) -> Flag {
  if (!GUI::GET::committed(page, id.c_str())) return false;
  Metre typed = row;
  if (::split(GUI::GET::text(page, id.c_str()), typed))
    COMMANDS::metred(row.at, typed.numerator, typed.denominator);
  GUI::edit(page, "");
  return true;
}

auto SOUND::VIEWS::CHANGES::metred(Whole from, Whole to) -> Flag {
  const Metre held = TRANSPORT::metred(from);
  HISTORY::begin();
  const Flag carried =
    COMMANDS::metred(to, held.numerator, held.denominator) &&
    COMMANDS::metred(from);
  HISTORY::end();
  return carried;
}

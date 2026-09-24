// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>
#include <sstream>

#include "session.doors.hpp"
#include "session.store.internal.hpp"

namespace {

Vector<SOUND::Husk> kept;
FIELDS::Map carried;

auto said(const FIELDS::Value &value) -> String {
  if (const auto *flag = std::get_if<Flag>(&value))
    return *flag ? "true" : "false";
  if (const auto *number = std::get_if<Integer>(&value))
    return std::format("{}", *number);
  if (const auto *number = std::get_if<Whole>(&value))
    return std::format("{}", *number);
  if (const auto *number = std::get_if<Float>(&value))
    return std::format("{}", *number);
  const auto *text = std::get_if<String>(&value);
  return text != nullptr ? *text : String();
}

}  // namespace

void SOUND::SESSION::husked(const String &line, Whole at, Vector<Husk> &husks) {
  if (line.empty()) return;
  husks.push_back({.at = at, .line = line});
}

void SOUND::SESSION::adopted(
  const Vector<Husk> &lines, const FIELDS::Map &fields) {
  ::kept = lines;
  ::carried = fields;
}

auto SOUND::SESSION::unread() -> const FIELDS::Map & { return ::carried; }

auto SOUND::SESSION::spliced(const String &text, STRING::Hot half) -> String {
  Vector<String> rows;
  std::istringstream lines(text);
  for (String row; std::getline(lines, row);) rows.push_back(row);
  for (const Husk &husk : ::kept) {
    if (husk.half != half) continue;
    const Whole at = std::min<Whole>(husk.at, Whole(rows.size()));
    rows.insert(std::next(rows.begin(), at), husk.line);
  }
  String back;
  for (const String &row : rows) back += row + "\n";
  return back;
}

auto SOUND::SESSION::husks() -> Vector<Husk> {
  Vector<Husk> shown;
  for (const auto &[field, value] : ::carried)
    shown.push_back({.line = std::format("{} {}", field, ::said(value))});
  std::sort(shown.begin(), shown.end(), [](const Husk &one, const Husk &two) {
    return one.line < two.line;
  });
  for (const Husk &husk : ::kept) shown.push_back(husk);
  return shown;
}

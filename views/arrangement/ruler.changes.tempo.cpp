// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <charconv>
#include <format>

#include "../../history.hpp"
#include "../../transport.hpp"
#include "ruler.changes.hpp"

namespace {
using namespace SOUND;

constexpr Whole SMOOTH = 0;
constexpr Float NOWHERE = 0.0f;

Flag dialling = false, moved = false;

auto read(const String &run, Float &value) -> Flag {
  const char *end = run.data() + run.size();
  const auto [last, fault] = std::from_chars(run.data(), end, value);
  return fault == std::errc() && last == end;
}

void turned(Whole at, Float tempo) {
  if (!::dialling) HISTORY::begin();
  ::dialling = ::moved = true;
  COMMANDS::paced(at, tempo);
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::CHANGES::beside(const Tempo &) -> Float {
  return ::NOWHERE;
}

auto SOUND::VIEWS::ARRANGEMENT::CHANGES::worded(const Tempo &row) -> String {
  return std::format("{:g}", row.tempo);
}

void SOUND::VIEWS::ARRANGEMENT::CHANGES::dressed(
  GUI::Handle page, const String &id, const Tempo &row) {
  const Float resting = TRANSPORT::held().tempos.front().tempo;
  GUI::set(
    page, id.c_str(),
    GUI::Dial{
      TRANSPORT::SLOWEST, TRANSPORT::FASTEST, resting, ::SMOOTH,
      GUI::Dial::DIAL});
  GUI::set(page, id.c_str(), GUI::Value{row.tempo});
}

auto SOUND::VIEWS::ARRANGEMENT::CHANGES::taken(
  GUI::Handle page, const String &id, const Tempo &row) -> Flag {
  if (GUI::GET::dialled(page, id.c_str())) {
    if (GUI::GET::editing(page) == id) GUI::edit(page, "");
    ::turned(row.at, GUI::GET::value(page, id.c_str()));
    return true;
  }
  if (!GUI::GET::committed(page, id.c_str())) return false;
  Float tempo = row.tempo;
  if (::read(GUI::GET::text(page, id.c_str()), tempo))
    COMMANDS::paced(
      row.at, std::clamp(tempo, TRANSPORT::SLOWEST, TRANSPORT::FASTEST));
  GUI::edit(page, "");
  return true;
}

void SOUND::VIEWS::ARRANGEMENT::CHANGES::settled() {
  if (::dialling && !::moved) {
    HISTORY::end();
    ::dialling = false;
  }
  ::moved = false;
}

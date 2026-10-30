// SPDX-License-Identifier: AGPL-3.0-or-later
#include <charconv>
#include <format>

#include "../../timeline.hpp"
#include "../lane/lane.face.hpp"
#include "../views.split.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;
namespace ARRANGER = SOUND::VIEWS::ARRANGER;

constexpr STRING::Hot WORD = "edit";
constexpr STRING::Hot HELP = "opens lane editor: edit <lane>";
constexpr STRING::Hot ANSWER = "edit track {} lane {} {}";
constexpr STRING::Hot ABSENT = "edit refused: no such lane";
constexpr STRING::Hot BARE = "edit refused: lane lacks editor";
constexpr STRING::Hot ATTENDED = "tracks";
constexpr Whole SHOWN = 1;
constexpr Whole SAID = 2;

auto laned(Whole track, Whole lane) -> const ARRANGEMENT::TRACK::Lane * {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (track >= tracks.size() || lane >= tracks[track].lanes.size())
    return nullptr;
  return &tracks[track].lanes[lane];
}

auto number(const String &word) -> Whole {
  Whole n = NONE;
  const char *end = word.data() + word.size();
  const auto [stop, fault] = std::from_chars(word.data(), end, n);
  return fault == std::errc{} && stop == end ? n : NONE;
}

void edit(SHELL::Session &session) {
  const Whole track = VIEWS::cursor(::ATTENDED);
  const Vector<String> &words = session.arguments;
  const Whole lane = words.size() == ::SAID ? ::number(words[1]) : NONE;
  const ARRANGEMENT::TRACK::Lane *one = ::laned(track, lane);
  if (one == nullptr) return session.print(::ABSENT);
  if (!ARRANGER::edit(track, lane)) return session.print(::BARE);
  session.print(std::format(::ANSWER, track, lane, KIND::spoken(one->kind)));
}

}  // namespace

auto SOUND::VIEWS::ARRANGER::editable(Whole kind) -> Flag {
  return LANE::editable(kind);
}

auto SOUND::VIEWS::ARRANGER::edit(Whole track, Whole lane) -> Flag {
  const ARRANGEMENT::TRACK::Lane *one = ::laned(track, lane);
  if (one == nullptr) return false;
  if (!LANE::editable(one->kind)) return false;
  LANE::mark(track, lane);
  const Whole region = SPLIT::open() ? SPLIT::focused() : SPLIT::UPPER;
  turned(region, one->kind);
  return region == SPLIT::BELOW ? SPLIT::choose(NAME) : VIEWS::choose(NAME);
}

auto SOUND::VIEWS::ARRANGER::velocity(Whole track, Whole lane) -> Flag {
  if (!edit(track, lane)) return false;
  LANE::faced(LANE::edited(KIND::NOTES)).loudness = ::SHOWN;
  return true;
}

auto SOUND::VIEWS::ARRANGER::words() -> Vector<SHELL::Command> {
  Vector<SHELL::Command> said = {{::WORD, ::HELP, ::edit}};
  for (const SHELL::Command &word : LANE::words()) said.push_back(word);
  return said;
}

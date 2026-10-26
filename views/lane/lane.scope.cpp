// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <charconv>
#include <format>
#include <utility>

#include "../../timeline.hpp"
#include "lane.internal.hpp"

namespace {
using namespace SOUND;
namespace LANE = SOUND::VIEWS::LANE;

constexpr STRING::Hot WORD = "scope";
constexpr STRING::Hot HELP = "scopes lane editor: scope [placement]";
constexpr STRING::Hot ANSWER = "scope {}";
constexpr STRING::Hot ABSENT = "none";
constexpr STRING::Hot REFUSED = "scope refused: no such placement";
constexpr STRING::Hot LINE = "lane scope {}";
constexpr Whole SAID = 2;

Vector<std::pair<String, String>> once;

auto number(const String &word) -> Whole {
  Whole n = NONE;
  const char *end = word.data() + word.size();
  const auto [stop, fault] = std::from_chars(word.data(), end, n);
  return fault == std::errc{} && stop == end ? n : NONE;
}

auto spoken(Whole clip) -> String {
  return clip == NONE ? String(::ABSENT) : std::format("{}", clip);
}

void scope(SHELL::Session &session) {
  const Vector<String> &words = session.arguments;
  if (words.size() > ::SAID) return session.print(::REFUSED);
  const Whole clip = words.size() == ::SAID ? ::number(words[1]) : NONE;
  if (words.size() == ::SAID && clip == NONE) return session.print(::REFUSED);
  if (!LANE::scope(clip)) return session.print(::REFUSED);
  session.print(std::format(::ANSWER, ::spoken(clip)));
}

auto seen(const LANE::Face &one) -> Flag {
  return std::ranges::find(::once, std::pair{one.track, one.lane}) !=
         ::once.end();
}

}  // namespace

auto SOUND::VIEWS::LANE::lane(const Face &one)
  -> const ARRANGEMENT::TRACK::Lane * {
  for (const ARRANGEMENT::Track &track : TIMELINE::held().tracks)
    for (const ARRANGEMENT::TRACK::Lane &lane : track.lanes)
      if (track.name == one.track && lane.name == one.lane) return &lane;
  return nullptr;
}

auto SOUND::VIEWS::LANE::scope(Whole clip) -> Flag {
  Face *one = held();
  if (one == nullptr) return false;
  const ARRANGEMENT::TRACK::Lane *row = lane(*one);
  if (clip != NONE && (row == nullptr || clip >= row->clips.size()))
    return false;
  if (!::seen(*one) && (clip != NONE || one->clip != NONE))
    ::once.emplace_back(one->track, one->lane);
  one->clip = clip;
  return true;
}

auto SOUND::VIEWS::LANE::words() -> Vector<SHELL::Command> {
  return {{::WORD, ::HELP, ::scope}};
}

auto SOUND::VIEWS::LANE::GET::scoped() -> const Clip * {
  const Face *one = held();
  if (one == nullptr || one->clip == NONE) return nullptr;
  const ARRANGEMENT::TRACK::Lane *row = lane(*one);
  if (row == nullptr || one->clip >= row->clips.size()) return nullptr;
  return &row->clips[one->clip];
}

auto SOUND::VIEWS::LANE::GET::admitted(Whole at) -> Flag {
  const Clip *clip = scoped();
  return clip == nullptr || (at >= clip->at && at < clip->at + clip->frames);
}

void SOUND::VIEWS::LANE::scoped(SHELL::Session &session) {
  const Face *one = held();
  if (one == nullptr || (one->clip == NONE && !::seen(*one))) return;
  session.print(std::format(::LINE, ::spoken(one->clip)));
}

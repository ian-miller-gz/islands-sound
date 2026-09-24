// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>

#include "../transport.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot RUNNING = "on";
constexpr STRING::Hot RESTING = "off";
constexpr STRING::Hot USAGE =
  "loop <from> <to> | loop <n> <text ...> | loop on | loop off";
constexpr STRING::Hot FIGURES = "0123456789";

auto counted(const String &word) -> Whole {
  return std::strtoul(word.c_str(), nullptr, 10);
}

auto tail(const SHELL::Session &session, Whole first) -> String {
  String text;
  for (Whole at = first; at < session.arguments.size(); ++at)
    text += (at == first ? String() : String(" ")) + session.arguments[at];
  return text;
}

auto worded(const SHELL::Session &session) -> Flag {
  return session.arguments.size() > 3 ||
         session.arguments[2].find_first_not_of(::FIGURES) != String::npos;
}

auto renamed(Whole index, const String &text) -> Flag {
  const Vector<Span> &loops = TRANSPORT::held().loops;
  if (index >= loops.size()) return false;
  return COMMANDS::looped(loops[index].from, loops[index].to, text);
}

}  // namespace

void SOUND::COMMANDS::cycle(SHELL::Session &session) {
  const Whole words = session.arguments.size();
  const Flag word = words == 2;
  if (word && session.arguments[1] == ::RESTING)
    TRANSPORT::loop(false);
  else if (word && session.arguments[1] == ::RUNNING)
    TRANSPORT::loop(true);
  else if (words >= 3 && ::worded(session)) {
    if (!::renamed(::counted(session.arguments[1]), ::tail(session, 2)))
      return session.print(::USAGE);
    return loops(session);
  } else if (words >= 3) {
    const Whole from = ::counted(session.arguments[1]);
    const Whole to = ::counted(session.arguments[2]);
    if (!looped(from, to, String())) return session.print(::USAGE);
    TRANSPORT::loop(true);
  } else if (words > 1)
    return session.print(::USAGE);
  else
    loops(session);
  timing(session);
}

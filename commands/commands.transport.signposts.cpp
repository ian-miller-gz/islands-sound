// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../transport.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot RESTING = "off";
constexpr STRING::Hot TAGGING = "tag <at> [text ...]";
constexpr STRING::Hot UNTAGGING = "untag <n>";
constexpr STRING::Hot UNLOOPING = "unloop <n>";
constexpr STRING::Hot ENDING = "ends <from> <to>";

auto counted(const String &word) -> Whole {
  return std::strtoul(word.c_str(), nullptr, 10);
}

auto tail(const SHELL::Session &session, Whole first) -> String {
  String text;
  for (Whole at = first; at < session.arguments.size(); ++at)
    text += (at == first ? String() : String(" ")) + session.arguments[at];
  return text;
}

void listed(SHELL::Session &session) {
  const Vector<Tag> &tags = TRANSPORT::held().tags;
  session.print(std::format("tags {}", tags.size()));
  for (Whole at = 0; at < tags.size(); ++at)
    session.print(
      std::format("tag {} at {} {}", at, tags[at].at, tags[at].text));
}

auto spelled() -> String {
  const Span ends = TRANSPORT::held().ends;
  return ends.to > ends.from
           ? std::format("ends from {} to {}", ends.from, ends.to)
           : std::format("ends {}", ::RESTING);
}

}  // namespace

void SOUND::COMMANDS::tag(SHELL::Session &session) {
  if (session.arguments.size() == 2) return session.print(::TAGGING);
  if (session.arguments.size() > 2)
    flagged(::counted(session.arguments[1]), ::tail(session, 2));
  ::listed(session);
}

void SOUND::COMMANDS::untag(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print(::UNTAGGING);
  const Vector<Tag> &tags = TRANSPORT::held().tags;
  const Whole index = ::counted(session.arguments[1]);
  if (index >= tags.size() || !flagged(tags[index].at))
    return session.print(::UNTAGGING);
  ::listed(session);
}

void SOUND::COMMANDS::loops(SHELL::Session &session) {
  const Vector<Span> &standing = TRANSPORT::held().loops;
  session.print(std::format("loops {}", standing.size()));
  for (Whole at = 0; at < standing.size(); ++at)
    session.print(std::format(
      "loop {} from {} to {}{}", at, standing[at].from, standing[at].to,
      standing[at].text.empty() ? String() : " " + standing[at].text));
}

void SOUND::COMMANDS::unloop(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print(::UNLOOPING);
  const Vector<Span> &standing = TRANSPORT::held().loops;
  const Whole index = ::counted(session.arguments[1]);
  if (index >= standing.size() || !looped(standing[index].from))
    return session.print(::UNLOOPING);
  loops(session);
  timing(session);
}

void SOUND::COMMANDS::ends(SHELL::Session &session) {
  if (session.arguments.size() == 2) return session.print(::ENDING);
  if (session.arguments.size() > 2)
    ended(::counted(session.arguments[1]), ::counted(session.arguments[2]));
  session.print(::spelled());
}

void SOUND::COMMANDS::ahead(SHELL::Session &session) {
  TRANSPORT::ahead();
  timing(session);
}

void SOUND::COMMANDS::behind(SHELL::Session &session) {
  TRANSPORT::behind();
  timing(session);
}

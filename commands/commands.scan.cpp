// SPDX-License-Identifier: AGPL-3.0-or-later
#include <charconv>
#include <format>

#include "../plugin.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot LINE = "scan {} roots {} found {}";
constexpr STRING::Hot ROOT = "root {} {}{}";
constexpr STRING::Hot FOUND = "found {} {}{}";
constexpr STRING::Hot REFUSED = "scan refused {}";
constexpr STRING::Hot OWN = " own";
constexpr STRING::Hot BUNDLED = " bundle";
constexpr STRING::Hot PLAIN = "";
constexpr STRING::Hot SPACE = " ";
constexpr Whole VERB = 1;
constexpr Whole OBJECT = 2;

auto object(const SHELL::Session &session) -> String {
  String joined;
  for (Whole at = ::OBJECT; at < session.arguments.size(); ++at)
    joined +=
      (at == ::OBJECT ? String() : String(::SPACE)) + session.arguments[at];
  return joined;
}

auto ran(const String &) -> Flag {
  PLUGIN::SCAN::run();
  return true;
}

auto added(const String &path) -> Flag { return PLUGIN::SCAN::add(path); }

auto dropped(const String &text) -> Flag {
  Whole index = 0;
  const auto [end, error] =
    std::from_chars(text.data(), text.data() + text.size(), index);
  return error == std::errc{} && end == text.data() + text.size() &&
         PLUGIN::SCAN::drop(index);
}

struct Verb {
  STRING::Hot name;
  Flag (*act)(const String &object);
};

constexpr Verb VERBS[] = {
  {"run", ::ran}, {"add", ::added}, {"drop", ::dropped}};

void rooted(
  SHELL::Session &session, const Vector<String> &roots, STRING::Hot mark) {
  for (Whole at = 0; at < roots.size(); ++at)
    session.print(std::format(::ROOT, at, roots[at], mark));
}

void reported(SHELL::Session &session) {
  const PLUGIN::SCAN::Report &report = PLUGIN::SCAN::held();
  session.print(
    std::format(::LINE, report.runs, report.roots.size(), report.found.size()));
  ::rooted(session, PLUGIN::SCAN::homes(), ::PLAIN);
  ::rooted(session, PLUGIN::SCAN::own(), ::OWN);
  for (const PLUGIN::SCAN::Found &one : report.found)
    session.print(std::format(
      ::FOUND, one.name, one.path, one.bundled ? ::BUNDLED : ::PLAIN));
}

}  // namespace

void SOUND::COMMANDS::scan(SHELL::Session &session) {
  if (session.arguments.size() > ::VERB) {
    const String object = ::object(session);
    for (const Verb &verb : ::VERBS)
      if (session.arguments[::VERB] == verb.name && !verb.act(object))
        return session.print(std::format(::REFUSED, object));
  }
  ::reported(session);
}

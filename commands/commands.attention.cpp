// SPDX-License-Identifier: AGPL-3.0-or-later
#include <charconv>
#include <format>
#include <iterator>

#include "../views.hpp"
#include "commands.offer.hpp"

namespace {
using namespace SOUND;
namespace ATTENTION = SOUND::VIEWS::ATTENTION;
namespace SPLIT = SOUND::VIEWS::SPLIT;

constexpr STRING::Hot COMMAND = "attention";
constexpr STRING::Hot HELP = "";
constexpr STRING::Hot HEAD = "attention {} {} {} {} focused {}";
constexpr STRING::Hot LINE = "slot {} track {} lane {} clip {} node {}";
constexpr STRING::Hot REFUSED = "attention refused: no such slot";
constexpr STRING::Hot ABSENT = "none";
constexpr STRING::Hot BARE = "-";
constexpr Whole FIRST = 1;
constexpr Whole BARED = 1;
constexpr Whole SAID = 3;

auto number(const String &text) -> Whole {
  Whole n = NONE;
  const char *end = text.data() + text.size();
  const auto [stop, fault] = std::from_chars(text.data(), end, n);
  return fault == std::errc{} && stop == end ? n : NONE;
}

auto spoken(Whole value) -> String {
  return value == NONE ? String(::ABSENT) : std::format("{}", value);
}

auto region(const String &name) -> Whole {
  for (Whole at = 0; at < std::size(SPLIT::REGIONS); ++at)
    if (name == SPLIT::REGIONS[at]) return at;
  return NONE;
}

void stated(SHELL::Session &session) {
  session.print(std::format(
    ::HEAD, SPLIT::REGIONS[SPLIT::UPPER],
    ATTENTION::held(SPLIT::UPPER) + ::FIRST, SPLIT::REGIONS[SPLIT::BELOW],
    ATTENTION::held(SPLIT::BELOW) + ::FIRST, SPLIT::REGIONS[SPLIT::focused()]));
  for (Whole n = 0; n < ATTENTION::SLOTS; ++n) {
    ATTENTION::Slot &one = ATTENTION::get(n);
    const String node = ATTENTION::aimed(one);
    session.print(std::format(
      ::LINE, n + ::FIRST, ::spoken(ATTENTION::attended(one)),
      ::spoken(one.mark.lane), ::spoken(one.clip),
      node.empty() ? String(::BARE) : node));
  }
}

void attention(SHELL::Session &session) {
  const Vector<String> &arguments = session.arguments;
  if (arguments.size() == ::SAID) {
    const Whole slot = ::number(arguments[2]);
    const Whole region = ::region(arguments[1]);
    const Flag known = slot != NONE && slot >= ::FIRST && region != NONE;
    if (!known || !ATTENTION::hold(region, slot - ::FIRST))
      return session.print(::REFUSED);
  } else if (arguments.size() != ::BARED)
    return session.print(::REFUSED);
  ::stated(session);
}

[[maybe_unused]] const Flag offered =
  COMMANDS::offer({{::COMMAND, ::HELP, ::attention}});

}  // namespace

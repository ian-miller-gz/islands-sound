// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "commands.internal.hpp"

namespace SOUND::COMMANDS {
namespace {

struct Word {
  STRING::Hot word;
  String (*read)(const Change &);
  Flag (*write)(const Change &, const String &);
};

const Word WORDS[] = {
  {HISTORY::WORD::NAME, CHANGE::named, CHANGE::named},
  {HISTORY::WORD::BUS, CHANGE::bussed, CHANGE::bussed},
  {HISTORY::WORD::HOME, CHANGE::homed, CHANGE::homed},
  {HISTORY::WORD::QUIET, CHANGE::quieted, CHANGE::quieted},
  {HISTORY::WORD::TEMPO, CHANGE::paced, CHANGE::paced},
  {HISTORY::WORD::METRE, CHANGE::metred, CHANGE::metred},
  {HISTORY::WORD::SCALE, CHANGE::keyed, CHANGE::keyed},
  {HISTORY::WORD::TAG, CHANGE::flagged, CHANGE::flagged},
  {HISTORY::WORD::LOOP, CHANGE::looped, CHANGE::looped},
  {HISTORY::WORD::ENDS, CHANGE::ended, CHANGE::ended}};

auto meant(const String &said) -> const Word * {
  for (const Word &one : WORDS)
    if (said == one.word) return &one;
  return nullptr;
}

}  // namespace
}  // namespace SOUND::COMMANDS

auto SOUND::COMMANDS::kept(Change change, const String &said) -> Flag {
  const String stood = stated(change);
  if (!state(change, change.now)) return false;
  change.stood = stood;
  HISTORY::record(
    {.act = change.word,
     .said = said,
     .verb = Edit::CHANGED,
     .changes = {change}});
  return true;
}

auto SOUND::COMMANDS::stated(const Change &change) -> String {
  const Word *word = meant(change.word);
  return word == nullptr ? String() : word->read(change);
}

auto SOUND::COMMANDS::state(const Change &change, const String &value) -> Flag {
  const Word *word = meant(change.word);
  return word != nullptr && word->write(change, value);
}

auto SOUND::COMMANDS::named(Whole track, Whole lane, const String &name)
  -> Flag {
  return kept(
    {.word = HISTORY::WORD::NAME,
     .at = lane == NONE ? std::format("{}", track)
                        : std::format("{} {}", track, lane),
     .now = name});
}

auto SOUND::COMMANDS::bussed(Whole track, Flag on) -> Flag {
  return kept(
    {.word = HISTORY::WORD::BUS,
     .at = std::format("{}", track),
     .now = on ? HISTORY::WORD::ON : HISTORY::WORD::OFF});
}

auto SOUND::COMMANDS::quieted(const String &node, Flag on) -> Flag {
  return kept(
    {.word = HISTORY::WORD::QUIET,
     .at = node,
     .now = on ? HISTORY::WORD::ON : HISTORY::WORD::OFF});
}

auto SOUND::COMMANDS::homed(
  const String &node, const String &page, Float across, Float down) -> Flag {
  return kept(
    {.word = HISTORY::WORD::HOME,
     .at = page.empty() ? node : node + " " + page,
     .now = std::format("{:g} {:g}", across, down)});
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "session.store.span.hpp"

using namespace SOUND;
using Clip = ARRANGEMENT::TRACK::LANE::Clip;
using Tune = ARRANGEMENT::TRACK::LANE::Tune;

auto SOUND::SESSION::tuned(const Clip &span) -> String {
  String text;
  for (const Tune &tune : span.tunes)
    text += std::format("pt {} {} {}\n", tune.at, tune.frames, tune.cents);
  return text;
}

void SOUND::SESSION::tune(std::istringstream &line, Clip &span) {
  Tune tune;
  if (line >> tune.at >> tune.frames >> tune.cents) span.tunes.push_back(tune);
}

void SOUND::SESSION::tune(
  std::istringstream &line, ARRANGEMENT::TRACK::Lane &lane) {
  if (!lane.outtakes.empty()) return tune(line, lane.outtakes.back());
  if (!lane.clips.empty()) tune(line, lane.clips.back());
}

void SOUND::SESSION::hung(
  const String &tag, std::istringstream &line, ARRANGEMENT::TRACK::Lane &lane) {
  if (tag == "pt")
    tune(line, lane);
  else
    (tag == "o" ? lane.outtakes : lane.clips).push_back(spanned(line));
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "piano.acts.hpp"
#include "piano.hands.hpp"

namespace {
constexpr STRING::Hot QUANTIZED = "quantize";
}  // namespace

void SOUND::VIEWS::ROLL::quantized(GUI::Handle page) {
  if (bench().snap == 0) return;
  Vector<Landing> landings;
  for (const Pip &pip : seated(page)) {
    const Integer by = Integer(HANDS::settled(pip.at)) - Integer(pip.at);
    const Whole at = clipped(pip, by);
    if (by != 0 && at != NONE) landings.push_back({pip, at, pip.pitch});
  }
  if (!landings.empty()) landed(landings, ::QUANTIZED);
}

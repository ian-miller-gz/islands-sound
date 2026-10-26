// SPDX-License-Identifier: AGPL-3.0-or-later
#include "piano.internal.hpp"
#include "piano.notation.hpp"

void SOUND::VIEWS::ROLL::ROWS::banded(
  GUI::Handle page, STRING::Hot stem, const Vector<Keyed> &sections) {
  const Flag tonal = ROLL::GET::notation() == TONAL;
  for (Whole section = 0; section < sections.size(); ++section) {
    const Keyed &laid = sections[section];
    for (Whole pitch = 0; pitch <= HIGHEST; ++pitch) {
      const String id = sectioned(stem, section, pitch);
      const Float y = lane(pitch, laid.key.at);
      GUI::set(page, id.c_str(), GUI::Visibility{tonal && y != OFF});
      GUI::set(page, id.c_str(), GUI::Position{laid.from, y});
      GUI::set(page, id.c_str(), GUI::Extent{laid.to - laid.from, STEP});
    }
  }
}

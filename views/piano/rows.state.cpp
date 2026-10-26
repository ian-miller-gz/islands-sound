// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "piano.internal.hpp"

namespace {
using namespace SOUND;
namespace ROWS = SOUND::VIEWS::ROLL::ROWS;

constexpr STRING::Hot ABSENT = "none";
constexpr STRING::Hot SEPARATOR = ",";

auto said(const Vector<Whole> &pitches) -> String {
  String line;
  for (Whole pitch : pitches)
    line += (line.empty() ? "" : ::SEPARATOR) + std::to_string(pitch);
  return line.empty() ? String(::ABSENT) : line;
}

}  // namespace

void SOUND::VIEWS::ROLL::ROWS::rows(SHELL::Session &session) {
  if (!folded()) return;
  const Vector<Keyed> sections = keyed({});
  for (Whole section = 0; section < sections.size(); ++section) {
    const Key &key = sections[section].key;
    session.print(
      std::format("rows {} section {}", shown(section).size(), section));
    session.print(
      std::format("plus {} added {}", gaps(key).size(), ::said(added(key))));
  }
}

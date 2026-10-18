// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>
#include <format>

#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STEM = "rail";
constexpr STRING::Hot LABELED = "label";
constexpr Float BYTE = 255.0f;
constexpr Float ORIGIN = 0.0f;

auto byte(Float channel) -> Whole {
  return Whole(std::lround(channel * ::BYTE));
}

auto hex(GFX::Color color) -> String {
  return std::format(
    "#{:02x}{:02x}{:02x}{:02x}", ::byte(color.r), ::byte(color.g),
    ::byte(color.b), ::byte(color.a));
}

void said(SHELL::Session &session, const String &id, const String &dress) {
  const GUI::Handle page = VIEWS::document();
  const String label = id + "." + ::LABELED;
  session.print(std::format(
    "dress {} {} fill {} hover {} press {} ink {}", id, dress,
    ::hex(GUI::GET::color(page, id.c_str())),
    ::hex(GUI::GET::hover(page, id.c_str())),
    ::hex(GUI::GET::press(page, id.c_str())),
    ::hex(GUI::GET::color(page, label.c_str()))));
}

}  // namespace

void SOUND::VIEWS::ROLL::dressed(SHELL::Session &session) {
  const String over = String(GUI::GET::hover(document()));
  const Vector<Keyed> sections = keyed(::ORIGIN);
  for (Whole section = 0; section < sections.size(); ++section)
    for (Whole pitch = 0; pitch <= HIGHEST; ++pitch)
      if (sectioned(::STEM, section, pitch) == over)
        return ::said(session, over, dressed(pitch, sections[section].key));
}

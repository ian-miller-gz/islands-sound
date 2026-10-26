// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "../../boards.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STEM = "ribbon";
constexpr STRING::Hot INK = "ink";
constexpr Float LETTER = 15.0f;

Whole stood = 0;
Whole born = 0;

auto ribbon(Whole track) -> String {
  return std::format("{}.{}.{}", VIEWS::ARRANGEMENT::board(), ::STEM, track);
}

void build(Whole count) {
  const GUI::Handle page = VIEWS::document();
  for (Whole track = count; track < ::stood; ++track)
    GUI::NODES::remove(page, ::ribbon(track).c_str());
  for (Whole track = ::stood; track < count; ++track) {
    const String id = ::ribbon(track);
    BOARDS::place(
      page, VIEWS::ARRANGEMENT::board().c_str(), "label", id, {},
      {VIEWS::ARRANGEMENT::NAMED, ::LETTER});
    GUI::set(page, id.c_str(), GUI::Style{::INK});
    GUI::set(page, id.c_str(), GUI::Size{::LETTER});
    GUI::set(page, id.c_str(), GUI::Depth{VIEWS::ARRANGEMENT::CREST});
    GUI::NGA::set(page, id.c_str(), GUI::NGA::Pinned{GUI::NGA::Pinned::ACROSS});
  }
  ::stood = count;
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::names() {
  const GUI::Handle page = VIEWS::document();
  const Vector<Ribbon> runs = ribbons();
  if (VIEWS::reborn(::born)) ::stood = 0;
  if (runs.size() != ::stood) ::build(runs.size());
  for (Whole track = 0; track < runs.size(); ++track) {
    const String id = ::ribbon(track);
    GUI::set(
      page, id.c_str(),
      GUI::Position{
        VIEWS::ARRANGEMENT::INSET,
        runs[track].top + (VIEWS::ARRANGEMENT::TITLE - ::LETTER) / 2.0f});
    GUI::set(page, id.c_str(), GUI::Text{runs[track].name});
  }
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "wave.internal.hpp"
#include "wave.rows.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ROWS = "wave.rows";
constexpr STRING::Hot TOOLS = "wave.tools";
constexpr STRING::Hot DEPTHS[] = {TOOLS, "arranger.palette"};

struct Grown {
  Whole born = 0;
  Float by = 0.0f;
};

Grown grown;

auto inside(GUI::Handle page) -> Float {
  const Float tools = GUI::GET::extent(page, ::TOOLS).h - ::grown.by;
  return tools - GUI::GET::position(page, ::ROWS).y -
         GUI::GET::extent(page, ::ROWS).h;
}

auto needed(GUI::Handle page, Whole rows) -> Float {
  if (rows == 0) return 0.0f;
  const Float pad = GUI::GET::pad(page, ::ROWS);
  return Float(rows) * (GUI::GET::pitch(page, ::ROWS) + pad) - pad;
}

}  // namespace

void SOUND::VIEWS::WAVE::fit(GUI::Handle page, Whole rows) {
  if (VIEWS::reborn(::grown.born)) ::grown = {.born = ::grown.born};
  const Float by = std::max(::needed(page, rows) - ::inside(page), ORIGIN);
  if (by == ::grown.by) return;
  for (STRING::Hot id : ::DEPTHS) {
    const GUI::Extent box = GUI::GET::extent(page, id);
    GUI::set(page, id, GUI::Extent{box.w, box.h + by - ::grown.by});
  }
  ::grown.by = by;
}

void SOUND::VIEWS::WAVE::SHED::band() { fit(document(), 0); }

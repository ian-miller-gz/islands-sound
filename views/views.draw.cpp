// SPDX-License-Identifier: AGPL-3.0-or-later
#include <utility>

#include <metrics.hpp>

#include <island/graphics/backend/passes.hpp>

#include "../boards.hpp"
#include "views.draw.hpp"
#include "views.internal.hpp"
#include "views.split.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot NIGHT = "night";
constexpr STRING::Hot DAY = "day";

auto ground() -> GFX::Color {
  const STRING::Hot dress = GUI::GET::theme() == GUI::DARK ? NIGHT : DAY;
  return GUI::GET::style(VIEWS::sheet(), dress).color;
}

}  // namespace

void SOUND::VIEWS::draw(const GFX::Viewport &viewport) {
  METRICS::gauge("sound.nodes", METRICS::Sample(BOARDS::seated()));
  METRICS::count("sound.restructured", GUI::GET::restructured(sheet()));
  GFX::Pass pass = {.clear = ::ground(), .viewport = viewport};
  SPLIT::draw(pass);
  {
    METRICS::Scope span("sound.flush");
    GUI::flush(sheet(), pass);
  }
  GFX::PASSES::submit(std::move(pass));
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../control.hpp"
#include "../../graph.hpp"
#include "../../kind.hpp"
#include "../../render.hpp"
#include "../../timeline.hpp"
#include "../../transport.hpp"
#include "signals.overridden.hpp"
#include "signals.plotted.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot DOOR = "signals.restore";
constexpr STRING::Hot WORN = "overridden";
constexpr STRING::Hot LIVE = "live";
constexpr STRING::Hot SHOWN = "shown";
constexpr STRING::Hot HIDDEN = "hidden";
constexpr STRING::Hot NOTHING = "none";

auto instant() -> Whole {
  const TRANSPORT::Marker marker = TRANSPORT::marker();
  return marker.playing ? marker.position : TRANSPORT::mark();
}

auto said(Flag on) -> String {
  return on ? VIEWS::SIGNALS::reading(VIEWS::SIGNALS::sounded())
            : String(::NOTHING);
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::target() -> Address {
  if (paged() == KIND::LOGIC) return addressed();
  if (paged() != KIND::CONTROL || number() == NONE) return {};
  return {.node = governs(), .parameter = number()};
}

auto SOUND::VIEWS::SIGNALS::overridden() -> Flag {
  const Address address = target();
  return !address.node.empty() && CONTROL::overridden(address);
}

auto SOUND::VIEWS::SIGNALS::worn(STRING::Hot stem) -> String {
  return overridden() ? String(::WORN) : dressed(stem);
}

auto SOUND::VIEWS::SIGNALS::sounded() -> Float {
  const Address address = target();
  const TIMELINE::Governed governed = RENDER::governed(address, ::instant());
  return CONTROL::sounding(
           address,
           {.governed = governed.stated,
            .curve = governed.value,
            .memory = GRAPH::remembered(address.node, address.parameter)})
    .value;
}

void SOUND::VIEWS::SIGNALS::restore() {
  const GUI::Handle page = document();
  const Flag on = overridden();
  GUI::set(page, ::DOOR, GUI::Visibility{on});
  read(on);
  if (on && GUI::GET::clicked(page, ::DOOR)) CONTROL::restore(target());
}

void SOUND::VIEWS::SIGNALS::restore(SHELL::Session &session) {
  const Flag on = overridden();
  session.print(std::format(
    "segments {} restore {} reads {}", on ? ::WORN : ::LIVE,
    on ? ::SHOWN : ::HIDDEN, ::said(on)));
}

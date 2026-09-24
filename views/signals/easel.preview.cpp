// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../graph.hpp"
#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "../../transport.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

Flag handing = false;
Whole indexed = NONE;
Float sounded = 0.0f;

auto rooted() -> String {
  const Whole track = VIEWS::SIGNALS::attended();
  if (track == NONE) return {};
  const String &root = TIMELINE::held().tracks[track].root;
  return GRAPH::rooted(root) ? root : String();
}

}  // namespace

void SOUND::VIEWS::SIGNALS::preview(const GUI::SAC::STROKE::Gesture &gesture) {
  if (gesture.opened || gesture.closed) ::handing = false;
  if (!gesture.standing || paged() != KIND::CONTROL) return;
  if (!TRANSPORT::marker().playing) return;
  const Whole lane = laned();
  const String root = ::rooted();
  if (lane == NONE || root.empty()) return;
  if (aimed(stamped(gesture.to.cell.across)).stock == NONE) return;
  const Float value = delivered(valued(gesture.to.cell.down));
  if (::handing && ::indexed == number() && ::sounded == value) return;
  if (!GRAPH::hand(
        root, lane, {AUDIO::PLUGIN::Event::CONTROLLER, 0, number(), value}))
    return;
  ::handing = true;
  ::indexed = number();
  ::sounded = value;
}

void SOUND::VIEWS::SIGNALS::preview(SHELL::Session &session) {
  if (!::handing) return;
  session.print(std::format(
    "easel preview number {} value {:.3f} run {}", ::indexed, ::sounded,
    GRAPH::handed(::rooted()).size));
}

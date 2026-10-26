// SPDX-License-Identifier: AGPL-3.0-or-later
#include "params.internal.hpp"

namespace {
auto zero(const AUDIO::PLUGIN::Control &published) -> Float {
  return (0.0f - published.least) / (published.most - published.least);
}
}  // namespace

auto SOUND::VIEWS::RACK::centred(const AUDIO::PLUGIN::Control &published)
  -> Flag {
  return published.least < 0.0f && 0.0f < published.most &&
         published.resting == 0.0f;
}

void SOUND::VIEWS::RACK::mark(
  GUI::Handle page, const String &cell,
  const AUDIO::PLUGIN::Control &published) {
  const String id = cell + "." + MARKED;
  const String dial = cell + "." + DIALLED;
  const Flag centre =
    centred(published) &&
    GUI::GET::dial(page, dial.c_str()).graphic == GUI::Dial::BAR;
  const GUI::Extent bar = GUI::GET::measured(page, dial.c_str());
  GUI::set(page, id.c_str(), GUI::Visibility{centre});
  GUI::set(
    page, id.c_str(),
    centre ? GUI::Seat{.beside = dial, .side = GUI::BELOW, .air = -bar.h}
           : GUI::Seat{});
  GUI::set(
    page, id.c_str(),
    GUI::Position{.x = centre ? bar.w * ::zero(published) : 0.0f, .y = 0.0f});
}

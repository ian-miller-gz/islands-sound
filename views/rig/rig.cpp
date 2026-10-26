// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cctype>
#include <format>

#include "rig.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot PATTERN = "rig.dial";
constexpr char POINT = '.';

auto numbered(const AUDIO::PLUGIN::Control &published) -> Flag {
  return published.steps == 0 || published.labels.size() != published.steps + 1;
}

auto digit(char letter) -> Flag {
  return std::isdigit(static_cast<unsigned char>(letter)) != 0;
}

auto graphic(GUI::Handle page, const AUDIO::PLUGIN::Control &published)
  -> GUI::Dial::Graphic {
  if (VIEWS::RIG::listed(published)) return GUI::Dial::NONE;
  if (published.steps == 1) return GUI::Dial::BAR;
  return GUI::GET::dial(page, ::PATTERN).graphic;
}

}  // namespace

auto SOUND::VIEWS::RIG::dressed(
  GUI::Handle page, const AUDIO::PLUGIN::Control &published) -> GUI::Dial {
  return {
    .least = published.least,
    .most = published.most,
    .resting = published.resting,
    .steps = published.steps,
    .graphic = ::graphic(page, published)};
}

auto SOUND::VIEWS::RIG::worded(
  const AUDIO::PLUGIN::Control &published, Float value) -> String {
  if (::numbered(published))
    return std::format("{:g}{}", value, published.unit);
  return published.labels[stepped(published, value)];
}

auto SOUND::VIEWS::RIG::worded(
  const AUDIO::PLUGIN::Control &published, Float value,
  const String &reading) -> String {
  if (!::numbered(published) || reading.empty())
    return worded(published, value);
  return std::format("{:.{}f}{}", value, places(reading), published.unit);
}

auto SOUND::VIEWS::RIG::places(const String &reading) -> Whole {
  const auto point = std::find(reading.begin(), reading.end(), ::POINT);
  if (point == reading.end()) return 0;
  return Whole(std::find_if_not(point + 1, reading.end(), ::digit) - point - 1);
}

auto SOUND::VIEWS::RIG::stepped(
  const AUDIO::PLUGIN::Control &published, Float value) -> Whole {
  const Float span = published.most - published.least;
  const Float fraction = std::clamp(
    span > 0.0f ? (value - published.least) / span : value, 0.0f, 1.0f);
  return Whole(fraction * Float(published.steps) + 0.5f);
}

auto SOUND::VIEWS::RIG::valued(
  const AUDIO::PLUGIN::Control &published, Whole step) -> Float {
  const Float span = published.most - published.least;
  return published.least + span * Float(step) / Float(published.steps);
}

auto SOUND::VIEWS::RIG::listed(const AUDIO::PLUGIN::Control &published)
  -> Flag {
  return published.steps > 1 && published.labels.size() == published.steps + 1;
}

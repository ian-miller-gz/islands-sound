// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../graph.hpp"
#include "../../kind.hpp"
#include "../../plugin.hpp"
#include "../../render.hpp"
#include "../lane/lane.face.hpp"
#include "signals.plotted.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot NOTHING = "none";

auto describes(AUDIO::PLUGIN::Control &out) -> Flag {
  if (VIEWS::SIGNALS::paged() == KIND::LOGIC) {
    const Address aim = VIEWS::SIGNALS::addressed();
    return !aim.node.empty() &&
           GRAPH::INSTANCE::described(aim.node, aim.parameter, out);
  }
  const String node = VIEWS::SIGNALS::governs();
  const Whole number = VIEWS::SIGNALS::number();
  if (!node.empty()) return GRAPH::INSTANCE::described(node, number, out);
  return GRAPH::described(VIEWS::SIGNALS::mapped(), number, out);
}

auto counted(const String &node, const String &plugin) -> Whole {
  return node.empty() ? GRAPH::parameters(plugin) : VIEWS::parameters(node);
}

auto spelled(const String &node, const String &plugin, Whole number) -> String {
  return node.empty() ? GRAPH::named(plugin, number)
                      : VIEWS::named(node, number);
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::laned() -> Whole {
  return LANE::edited(paged()).lane;
}

auto SOUND::VIEWS::SIGNALS::mapped() -> String {
  const Whole lane = laned();
  return lane == NONE ? String() : RENDER::mapped(attended(), lane);
}

auto SOUND::VIEWS::SIGNALS::governs() -> String {
  const Whole lane = laned();
  return lane == NONE ? String() : RENDER::governs(attended(), lane);
}

auto SOUND::VIEWS::SIGNALS::named() -> String {
  if (paged() != KIND::LOGIC) return ::spelled(governs(), mapped(), number());
  const Address aim = addressed();
  if (aim.node.empty()) return ::NOTHING;
  return std::format("{} {}", aim.node, VIEWS::named(aim.node, aim.parameter));
}

auto SOUND::VIEWS::SIGNALS::unit() -> String {
  AUDIO::PLUGIN::Control published;
  return ::describes(published) ? published.unit : String();
}

auto SOUND::VIEWS::SIGNALS::delivered(Float value) -> Float {
  AUDIO::PLUGIN::Control published;
  return ::describes(published) ? PLUGIN::scaled(published, value) : value;
}

auto SOUND::VIEWS::SIGNALS::crossed(Float quantity) -> Float {
  AUDIO::PLUGIN::Control published;
  const Flag bound = ::describes(published) && published.most > published.least;
  const Float drawn = bound ? (quantity - published.least) /
                                (published.most - published.least) * Float(FULL)
                            : quantity;
  return std::clamp(drawn, 0.0f, Float(FULL));
}

auto SOUND::VIEWS::SIGNALS::spoken(Float value) -> String {
  return std::format("{:g}", delivered(value));
}

auto SOUND::VIEWS::SIGNALS::worded() -> Vector<Word> {
  const String node = governs(), plugin = mapped();
  const Whole listed = ::counted(node, plugin);
  Vector<Word> run;
  for (Whole number = 0; number <= FULL; ++number)
    if (number < listed)
      run.push_back(
        {.number = number, .name = ::spelled(node, plugin, number)});
  for (Whole number = 0; number <= FULL; ++number)
    if (number >= listed) run.push_back({.number = number});
  return run;
}

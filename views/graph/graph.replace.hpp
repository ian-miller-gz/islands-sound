// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "graph.browse.internal.hpp"

namespace SOUND::VIEWS::WIRED {

constexpr STRING::Hot WHOSE = "graph";
constexpr STRING::Hot REPLACES = "Replace";

void replacing(const String &node);

auto replaceable(const String &node) -> Flag;

auto replacing() -> String;

auto replace(const String &node, const String &plugin) -> Whole;

void take(const String &plugin, STRING::Hot list, Whole row);

void asking();

}  // namespace SOUND::VIEWS::WIRED

namespace SOUND::VIEWS::WIRED::BROWSE {

auto narrow(const String &filing, Vector<Offer> rows) -> Vector<Offer>;

auto titled() -> String;

}  // namespace SOUND::VIEWS::WIRED::BROWSE

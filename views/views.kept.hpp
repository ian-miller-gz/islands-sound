// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "views.hpp"

namespace SOUND::VIEWS::KEPT {

auto number(const FIELDS::Map &face, const String &key, Float &value) -> Flag;

auto number(const FIELDS::Map &face, const String &key, Whole &value) -> Flag;

auto text(const FIELDS::Map &face, const String &key, String &value) -> Flag;

auto join(const String &stem, Whole n, const String &leaf = {}) -> String;

void board(FIELDS::Map &face, const View &unit, const String &head = {});

void boarded(
  const FIELDS::Map &face, const View &unit, const String &head = {});

}  // namespace SOUND::VIEWS::KEPT

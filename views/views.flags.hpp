// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "views.hpp"

namespace SOUND::VIEWS {

constexpr Float FLY = 64.0f;
constexpr Float HOIST = 14.0f;
constexpr Float ENDED = 14.0f;
constexpr Float POLE = 2.0f;
constexpr STRING::Hot NAMED = "name";

struct Flight {
  Float place = 0.0f;
  Flag shown = true;
  Flag closing = false;
  Flag above = false;
  Float wide = FLY;
  Flag worded = false;
};

void hoist(
  const String &strip, const String &id, STRING::Hot dress, Flag worded);

void strike(const String &id);

auto region(const String &strip) -> String;

void word(const String &cloth, const String &stem);

void unword(const String &stem);

void worded(const String &cloth, const String &stem, const String &text);

void fly(const String &strip, const String &id, const Flight &flight);

auto planted(const String &strip, const String &id, Flag closing) -> Float;

}  // namespace SOUND::VIEWS

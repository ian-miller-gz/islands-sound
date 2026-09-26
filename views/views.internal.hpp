// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui/ladder.hpp>

#include "views.hpp"

namespace SOUND::VIEWS {

auto registry() -> Vector<View> &;

auto sheet() -> GUI::Handle &;

auto chosen() -> Whole &;
auto standings() -> Vector<Flag> &;

constexpr STRING::Hot MARK = "selected";
constexpr STRING::Hot PLAIN = "icon";

constexpr STRING::Hot HANDLED = "handle";
constexpr STRING::Hot BAR = "column.grip";

void show();

void rebirth();

void chose();

void doors();

void wheeled();

void file();
void exporting();

void settings();
auto asking() -> Flag;

void settled();

void dressed();

void remembered();

void deck();

void lift();

constexpr STRING::Hot CYCLE = "cycle";

constexpr STRING::Hot TAG = "tag";
constexpr STRING::Hot ENDS = "ends";

constexpr STRING::Hot OPENED = "from";
constexpr STRING::Hot CLOSED = "to";
constexpr STRING::Hot LABEL = "label";

constexpr Float BAND = 9.0f;
constexpr Float FLAG = 4.0f;

struct Post {
  enum Kind { CYCLED, TAGGED, BOUND };

  Kind kind = CYCLED;
  Whole at = 0;
  Whole place = 0;
  Flag closing = false;
  String id;
};

auto posts(const String &strip) -> Vector<Post>;

auto label(const Post &post) -> String;

void cycled(Integer at, Whole span);

void cycled();

auto framed(Whole pulses) -> Whole;
auto pulsed(Whole frames) -> Whole;

using Fold = Float (*)(Float frames, Float west, Float scale);

void flagged(
  const String &strip, Float west, Float scale, Float down, Fold fold);

void flagged(
  SHELL::Session &session, Whole cells, Float west, Float scale, Fold fold);

using Unfold = Whole (*)(Float place, Float west, Float scale);

auto carried() -> String &;

void handled(const String &strip, Float west, Float scale, Unfold unfold);

void offered(const Vector<Post> &posts);

using Pulsed = Whole (*)(Whole place);

void asked(
  const String &strip, const Vector<GUI::SAC::LADDER::Mark> &dressed,
  Whole span, Pulsed pulsed);

void column();

void marked(const String &map, Placed placed);
void marked(SHELL::Session &session, const String &map, Placed placed);

}  // namespace SOUND::VIEWS

namespace SOUND::VIEWS::KEPT {

auto number(const FIELDS::Map &face, const String &key, Float &value) -> Flag;

void board(FIELDS::Map &face, const View &unit);

void boarded(const FIELDS::Map &face, const View &unit);

}  // namespace SOUND::VIEWS::KEPT

namespace SOUND::VIEWS::ICONS {

void create();

}  // namespace SOUND::VIEWS::ICONS

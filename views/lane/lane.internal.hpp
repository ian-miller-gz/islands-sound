// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <deque>

#include "../views.hpp"
#include "lane.face.hpp"

namespace SOUND::ARRANGEMENT::TRACK {

struct Lane;

}  // namespace SOUND::ARRANGEMENT::TRACK

namespace SOUND::VIEWS::LANE {

auto faced() -> const View *;

auto faced(Whole kind) -> const View *;

auto field() -> String;

auto field(Whole kind) -> String;

auto windowed() -> Float;

auto map() -> String;
auto slide() -> String;

constexpr Float RATIO = 0.1f;

void minimap();

void minimap(SHELL::Session &session);

void slider();

void strolled(GUI::Handle page, const String &id);

auto stored() -> Mark &;

auto records() -> std::deque<Face> &;

void keep(FIELDS::Map &face, Whole n, Face &one);

void restore(const FIELDS::Map &face, Whole n, Face &one);

void keep(FIELDS::Map &face, const String &stem, const Vector<Added> &added);

void restore(const FIELDS::Map &face, const String &stem, Vector<Added> &added);

void swap(Whole kind);

void record();

void forget();

auto held() -> Face *;

auto lane(const Face &one) -> const SOUND::ARRANGEMENT::TRACK::Lane *;

void bound();

void dim();
void dim(const Clip &clip, const Overview &read, Float rail);

auto spanned(GUI::Handle page, const String &id, Float wide) -> Flag;

void scoped(SHELL::Session &session);

namespace SHED {

void minimap();

}  // namespace SHED

}  // namespace SOUND::VIEWS::LANE

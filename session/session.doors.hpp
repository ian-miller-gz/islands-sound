// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <common/fields.hpp>

#include "session.hpp"

namespace SOUND::SESSION {

auto held() -> const Session &;

void seated(const String &plugin);

void adopt(const Session &document);

auto save(
  const String &name, const Setting &setting, const Arrangement &tracks,
  const Inventory &pool, const Graph &wiring, const Vector<Value> &values,
  const History &history) -> Flag;

auto open(const String &name) -> STRING::Hot;

auto rewritten() -> Whole;

auto husks() -> Vector<Husk>;

auto names() -> Vector<String>;

void keep(const String &name, const FIELDS::Map &face);

auto kept(const String &name, FIELDS::Map &face) -> Flag;

void close();

}  // namespace SOUND::SESSION

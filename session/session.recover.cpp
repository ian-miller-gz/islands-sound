// SPDX-License-Identifier: AGPL-3.0-or-later
#include <store.hpp>

#include "session.doors.hpp"
#include "session.store.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::SESSION;

constexpr STRING::Hot TABLE = "recovery";
constexpr STRING::Hot UNNAMED = "unnamed";

auto keyed(const String &name) -> String {
  return name.empty() ? String(::UNNAMED) : name;
}

}  // namespace

auto SOUND::SESSION::recovered(const String &name) -> Flag {
  FIELDS::Map row;
  const STORE::Handle opened = store();
  return opened.id != 0 &&
         STORE::get(opened, ::TABLE, ::keyed(name).c_str(), row);
}

auto SOUND::SESSION::recover(
  const String &name, const Setting &setting, const Arrangement &tracks,
  const Inventory &pool, const Graph &wiring, const Vector<Value> &values,
  const History &history) -> Flag {
  const STORE::Handle opened = store();
  if (opened.id == 0) return false;
  const Session document = {
    .name = name,
    .arrangement = tracks,
    .graph = wiring,
    .history = history,
    .inventory = pool,
    .setting = setting,
    .values = values,
    .recents = held().recents};
  STORE::put(opened, ::TABLE, ::keyed(name).c_str(), written(document));
  return true;
}

auto SOUND::SESSION::restore(const String &name) -> STRING::Hot {
  FIELDS::Map row;
  if (!STORE::get(store(), ::TABLE, ::keyed(name).c_str(), row))
    return "no recovery";
  Session document;
  document.name = name;
  Vector<Husk> husks;
  taken(row, schemed(row), document, husks);
  adopted(husks, unspoken(row));
  adopt(document);
  return "";
}

void SOUND::SESSION::forget(const String &name) {
  const STORE::Handle opened = store();
  if (opened.id != 0) STORE::erase(opened, ::TABLE, ::keyed(name).c_str());
}

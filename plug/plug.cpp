// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstring>

#include "plug.hpp"

namespace {

auto kept() -> Vector<SOUND::PLUG::Offer> & {
  static Vector<SOUND::PLUG::Offer> catalog;
  return catalog;
}

auto before(const SOUND::PLUG::Offer &one, const SOUND::PLUG::Offer &two)
  -> Flag {
  return std::strcmp(one.name, two.name) < 0;
}

}  // namespace

auto SOUND::PLUG::offer(const Offer &row) -> Flag {
  if (row.name == nullptr || row.name[0] == '\0') return false;
  if (row.surface == nullptr) return false;
  Vector<Offer> &catalog = ::kept();
  const auto at = std::lower_bound(catalog.begin(), catalog.end(), row, before);
  if (at != catalog.end() && std::strcmp(at->name, row.name) == 0) return false;
  catalog.insert(at, row);
  return true;
}

auto SOUND::PLUG::catalog() -> const Vector<Offer> & { return ::kept(); }

auto SOUND::PLUG::found(const String &name) -> const Offer * {
  for (const Offer &row : ::kept())
    if (name == row.name) return &row;
  return nullptr;
}

auto SOUND::PLUG::bound(const String &name) -> Flag {
  const Offer *row = found(name);
  return row != nullptr && row->bind != nullptr;
}

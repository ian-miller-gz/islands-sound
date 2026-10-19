// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "views.kept.hpp"

namespace {
using namespace SOUND;

template <typename Type>
auto taken(const FIELDS::Map &face, const String &key, Type &value) -> Flag {
  const auto found = face.find(key);
  if (found == face.end()) return false;
  const Type *held = std::get_if<Type>(&found->second);
  if (held != nullptr) value = *held;
  return held != nullptr;
}

}  // namespace

auto SOUND::VIEWS::KEPT::number(
  const FIELDS::Map &face, const String &key, Whole &value) -> Flag {
  return ::taken(face, key, value);
}

auto SOUND::VIEWS::KEPT::text(
  const FIELDS::Map &face, const String &key, String &value) -> Flag {
  return ::taken(face, key, value);
}

auto SOUND::VIEWS::KEPT::join(const String &stem, Whole n, const String &leaf)
  -> String {
  if (leaf.empty()) return std::format("{}.{}", stem, n);
  return std::format("{}.{}.{}", stem, n, leaf);
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <filesystem>

#include "../session.hpp"
#include "scan.hpp"

namespace {
using namespace SOUND;
using Path = std::filesystem::path;

constexpr STRING::Hot KEY = "scan.roots";
constexpr STRING::Hot FIELD = "roots";

Vector<String> roots;
Flag read = false;

auto taken() -> Vector<String> & {
  if (::read) return ::roots;
  ::read = true;
  FIELDS::Map row;
  if (!SESSION::settled(::KEY, row)) return ::roots;
  const auto at = row.find(::FIELD);
  if (at == row.end()) return ::roots;
  if (const String *joined = std::get_if<String>(&at->second))
    ::roots = PLUGIN::SCAN::split(*joined);
  return ::roots;
}

void kept() {
  String joined;
  for (const String &root : ::roots)
    joined +=
      (joined.empty() ? String() : String(1, PLUGIN::SCAN::SEPARATOR)) + root;
  SESSION::settle(::KEY, FIELDS::Map{{::FIELD, FIELDS::Value{joined}}});
}

auto normal(const String &path) -> String {
  std::error_code error;
  Path held = std::filesystem::absolute(path, error).lexically_normal();
  if (!held.has_filename()) held = held.parent_path();
  return held.string();
}

}  // namespace

auto SOUND::PLUGIN::SCAN::own() -> const Vector<String> & { return ::taken(); }

auto SOUND::PLUGIN::SCAN::add(const String &path) -> Flag {
  if (path.empty()) return false;
  const String root = ::normal(path);
  std::error_code error;
  if (!std::filesystem::is_directory(root, error)) return false;
  const Vector<String> held = roots();
  if (std::ranges::find(held, root) != held.end()) return false;
  ::taken().push_back(root);
  ::kept();
  return true;
}

auto SOUND::PLUGIN::SCAN::drop(Whole index) -> Flag {
  Vector<String> &held = ::taken();
  if (index >= held.size()) return false;
  held.erase(held.begin() + static_cast<Integer>(index));
  ::kept();
  return true;
}

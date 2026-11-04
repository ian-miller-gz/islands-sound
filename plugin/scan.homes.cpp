// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdlib>
#include <filesystem>

#include "scan.hpp"

namespace {
using namespace SOUND;
using Path = std::filesystem::path;

constexpr STRING::Hot LISTED = "VST3_PATH";

namespace VARIABLE {
constexpr STRING::Hot NONE = "";
constexpr STRING::Hot HOME = "HOME";
constexpr STRING::Hot COMMON = "COMMONPROGRAMFILES";
constexpr STRING::Hot LOCAL = "LOCALAPPDATA";
}  // namespace VARIABLE

namespace PLACE {
constexpr STRING::Hot NONE = "";
constexpr STRING::Hot USER = ".vst3";
constexpr STRING::Hot SYSTEM = "/usr/lib/vst3";
constexpr STRING::Hot LOCAL = "/usr/local/lib/vst3";
constexpr STRING::Hot LIBRARY = "/Library/Audio/Plug-Ins/VST3";
constexpr STRING::Hot AUDIO = "Library/Audio/Plug-Ins/VST3";
constexpr STRING::Hot FILES = "C:\\Program Files\\Common Files";
constexpr STRING::Hot COMMON = "VST3";
constexpr STRING::Hot PROGRAMS = "Programs\\Common\\VST3";
}  // namespace PLACE

struct Home {
  Whole platform;
  STRING::Hot variable, fallback, path;
};

constexpr Home HOMES[] = {
  {SR_POSIX, VARIABLE::HOME, PLACE::NONE, PLACE::USER},
  {SR_POSIX, VARIABLE::NONE, PLACE::NONE, PLACE::SYSTEM},
  {SR_POSIX, VARIABLE::NONE, PLACE::NONE, PLACE::LOCAL},
  {SR_POSIX, VARIABLE::NONE, PLACE::NONE, PLACE::LIBRARY},
  {SR_POSIX, VARIABLE::HOME, PLACE::NONE, PLACE::AUDIO},
  {SR_WINDOWS, VARIABLE::COMMON, PLACE::FILES, PLACE::COMMON},
  {SR_WINDOWS, VARIABLE::LOCAL, PLACE::NONE, PLACE::PROGRAMS},
};

void standing(Vector<String> &roots, const Path &root) {
  std::error_code error;
  if (!std::filesystem::is_directory(root, error)) return;
  const String held = root.string();
  if (std::ranges::find(roots, held) == roots.end()) roots.push_back(held);
}

void homed(Vector<String> &roots, const Home &home) {
  if (home.platform != SR_PLATFORM) return;
  if (*home.variable == '\0') return ::standing(roots, home.path);
  STRING::Hot base = std::getenv(home.variable);
  if (base == nullptr) base = home.fallback;
  if (*base != '\0') ::standing(roots, Path(base) / home.path);
}

}  // namespace

auto SOUND::PLUGIN::SCAN::split(const String &run) -> Vector<String> {
  Vector<String> entries;
  for (size_t at = 0; at <= run.size();) {
    const size_t end = std::min(run.find(SEPARATOR, at), run.size());
    if (end > at) entries.push_back(run.substr(at, end - at));
    at = end + 1;
  }
  return entries;
}

auto SOUND::PLUGIN::SCAN::homes() -> Vector<String> {
  Vector<String> roots;
  if (STRING::Hot listed = std::getenv(::LISTED))
    for (const String &entry : split(listed)) ::standing(roots, entry);
  for (const Home &home : ::HOMES) ::homed(roots, home);
  return roots;
}

auto SOUND::PLUGIN::SCAN::roots() -> Vector<String> {
  Vector<String> roots = homes();
  for (const String &root : own()) ::standing(roots, root);
  return roots;
}

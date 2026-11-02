// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <format>

#include <logger.hpp>

#include "scan.hpp"

namespace {
using namespace SOUND;
using Path = std::filesystem::path;

constexpr STRING::Hot CATEGORY = "~/sound/plugins";
constexpr STRING::Hot LISTED = "VST3_PATH";
constexpr STRING::Hot HOME = "HOME";
constexpr STRING::Hot OWN = ".vst3";
constexpr STRING::Hot SHARED[] = {"/usr/lib/vst3", "/usr/local/lib/vst3"};
constexpr char APART = ':';
constexpr int DEEPEST = 2;

PLUGIN::SCAN::Report report;

auto standing(Vector<String> &roots, const Path &root) {
  std::error_code error;
  if (!std::filesystem::is_directory(root, error)) return;
  const String word = root.string();
  if (std::ranges::find(roots, word) == roots.end()) roots.push_back(word);
}

void listed(Vector<String> &roots) {
  const char *listed = std::getenv(::LISTED);
  if (listed == nullptr) return;
  const String run(listed);
  for (size_t at = 0; at <= run.size();) {
    const size_t end = std::min(run.find(::APART, at), run.size());
    if (end > at) ::standing(roots, run.substr(at, end - at));
    at = end + 1;
  }
}

auto found(const std::filesystem::directory_entry &entry) -> Flag {
  return entry.path().extension().string() == PLUGIN::SCAN::EXTENSION;
}

void walked(const String &root, Vector<PLUGIN::SCAN::Found> &found) {
  std::error_code error, kind;
  std::filesystem::recursive_directory_iterator at(root, error), end;
  for (; !error && at != end; at.increment(error)) {
    if (::found(*at)) {
      found.push_back(
        {at->path().stem().string(), at->path().string(),
         at->is_directory(kind)});
      at.disable_recursion_pending();
    } else if (at.depth() >= ::DEEPEST)
      at.disable_recursion_pending();
  }
}

void logged(const PLUGIN::SCAN::Report &report) {
  LOGGER::Category &logger = LOGGER::get(::CATEGORY);
  for (const String &root : report.roots)
    logger.info(std::format("scanning {}", root));
  for (const PLUGIN::SCAN::Found &one : report.found)
    logger.info(std::format(
      "found {} at {}{}", one.name, one.path, one.bundled ? " (bundle)" : ""));
  logger.info(std::format(
    "scan {}: {} roots, {} vst3", report.runs, report.roots.size(),
    report.found.size()));
}

}  // namespace

auto SOUND::PLUGIN::SCAN::roots() -> Vector<String> {
  Vector<String> roots;
  ::listed(roots);
  if (const char *home = std::getenv(::HOME)) ::standing(roots, Path(home) / ::OWN);
  for (STRING::Hot shared : ::SHARED) ::standing(roots, shared);
  return roots;
}

auto SOUND::PLUGIN::SCAN::run() -> const Report & {
  ::report.roots = roots();
  ::report.found.clear();
  for (const String &root : ::report.roots) ::walked(root, ::report.found);
  std::ranges::stable_sort(::report.found, {}, &Found::name);
  ++::report.runs;
  ::logged(::report);
  return ::report;
}

auto SOUND::PLUGIN::SCAN::held() -> const Report & { return ::report; }

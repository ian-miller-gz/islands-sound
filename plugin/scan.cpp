// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <filesystem>
#include <format>

#include <logger.hpp>

#include "scan.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot CATEGORY = "~/sound/plugins";
constexpr int DEEPEST = 2;

PLUGIN::SCAN::Report report;

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

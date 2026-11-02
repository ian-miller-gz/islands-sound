// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <common.hpp>

namespace SOUND::PLUGIN::SCAN {

constexpr STRING::Hot EXTENSION = ".vst3";

/// @brief One VST3 found on disk: its name, where it stands, and whether
/// it is a bundle directory or a single file.
struct Found {
  String name, path;
  Flag bundled = false;
};

/// @brief What the last scan read: the roots walked, what was found, and
/// how many scans have run.
struct Report {
  Vector<String> roots;
  Vector<Found> found;
  Whole runs = 0;
};

/// @brief The roots a scan walks: `VST3_PATH`'s entries, then the
/// platform's own plugin directories, each only when it stands.
auto roots() -> Vector<String>;

/// @brief Walk the roots, log each find, and keep the report.
auto run() -> const Report &;

auto held() -> const Report &;

}  // namespace SOUND::PLUGIN::SCAN

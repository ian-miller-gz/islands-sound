// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <common.hpp>
#include <common/platform/selection.hpp>

namespace SOUND::PLUGIN::SCAN {

constexpr STRING::Hot EXTENSION = ".vst3";

struct Listing {
  Whole platform;
  char separator;
};

constexpr Listing LISTINGS[] = {
  {SR_POSIX, ':'}, {SR_WEB, ':'}, {SR_WINDOWS, ';'}};

constexpr char SEPARATOR = [] {
  for (const Listing &row : LISTINGS)
    if (row.platform == SR_PLATFORM) return row.separator;
  return LISTINGS[0].separator;
}();

struct Found {
  String name, path;
  Flag bundled = false;
};

struct Report {
  Vector<String> roots;
  Vector<Found> found;
  Whole runs = 0;
};

auto split(const String &run) -> Vector<String>;

auto homes() -> Vector<String>;

auto roots() -> Vector<String>;

auto own() -> const Vector<String> &;

auto add(const String &path) -> Flag;

auto drop(Whole index) -> Flag;

auto run() -> const Report &;

auto held() -> const Report &;

}  // namespace SOUND::PLUGIN::SCAN

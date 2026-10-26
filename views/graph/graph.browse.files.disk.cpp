// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>

#include "graph.browse.internal.hpp"

namespace {
using namespace SOUND;
using Path = std::filesystem::path;

constexpr STRING::Hot MUSIC = "XDG_MUSIC_DIR";
constexpr STRING::Hot HOME = "HOME";
constexpr STRING::Hot SHELF = "Music";
constexpr STRING::Hot EXTENSION = ".wav";
constexpr char HIDDEN = '.';

auto root() -> Path {
  if (const char *music = std::getenv(::MUSIC)) return music;
  const char *home = std::getenv(::HOME);
  if (home == nullptr) return {};
  std::error_code error;
  const Path shelf = Path(home) / ::SHELF;
  return std::filesystem::is_directory(shelf, error) ? shelf : Path(home);
}

auto waved(const Path &file) -> Flag {
  String extension = file.extension().string();
  for (char &letter : extension)
    letter = char(std::tolower(static_cast<unsigned char>(letter)));
  return extension == ::EXTENSION;
}

}  // namespace

auto SOUND::VIEWS::WIRED::BROWSE::read(const String &filing) -> Folder {
  const Path tail = Path(filing.substr(String(FILES).size())).relative_path();
  const Path folder = ::root() / tail;
  Folder held = {.path = (folder / Path()).string()};
  std::error_code error, kind;
  std::filesystem::directory_iterator at(folder, error), end;
  for (; !error && at != end; at.increment(error)) {
    const String name = at->path().filename().string();
    if (name.empty() || name.front() == ::HIDDEN) continue;
    if (at->is_directory(kind))
      held.folders.push_back(name);
    else if (::waved(at->path()))
      held.waves.push_back(name);
  }
  std::ranges::sort(held.folders);
  std::ranges::sort(held.waves);
  return held;
}

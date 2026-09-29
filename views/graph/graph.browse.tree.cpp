// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "graph.browse.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::VIEWS::WIRED::BROWSE;

constexpr STRING::Hot CATALOG = "Catalog";
constexpr STRING::Hot APART = " / ";

auto within(const String &path, const String &filing) -> Flag {
  if (filing.empty()) return !path.empty();
  const String stem = filing + WITHIN;
  return path.starts_with(stem) && path.size() > stem.size();
}

auto step(const String &path, const String &filing) -> String {
  const Whole from = filing.empty() ? 0 : filing.size() + String(WITHIN).size();
  return path.substr(0, path.find(WITHIN, from));
}

auto last(const String &path) -> String {
  const Whole at = path.rfind(WITHIN);
  return at == String::npos ? path : path.substr(at + String(WITHIN).size());
}

auto held(const String &directory, const Vector<Offer> &plugins) -> Whole {
  return std::count_if(plugins.begin(), plugins.end(), [&](const Offer &row) {
    const String path = filed(row);
    return path == directory || within(path, directory);
  });
}

}  // namespace

auto SOUND::VIEWS::WIRED::BROWSE::directories(
  const String &filing, const Vector<Offer> &plugins) -> Vector<Offer> {
  Vector<String> paths;
  if (filing.empty()) paths.assign(std::begin(STANDARDS), std::end(STANDARDS));
  for (const Offer &plugin : plugins)
    if (::within(filed(plugin), filing))
      paths.push_back(::step(filed(plugin), filing));
  std::sort(paths.begin(), paths.end());
  paths.erase(std::unique(paths.begin(), paths.end()), paths.end());
  Vector<Offer> rows;
  for (const String &path : paths)
    rows.push_back(
      {.name = ::last(path),
       .from = ::step(path, String()),
       .among = path,
       .directory = true,
       .holds = ::held(path, plugins)});
  return rows;
}

auto SOUND::VIEWS::WIRED::BROWSE::under(
  const String &filing, const Vector<Offer> &plugins) -> Vector<Offer> {
  Vector<Offer> rows;
  for (const Offer &plugin : plugins)
    if (filed(plugin) == filing) rows.push_back(plugin);
  std::sort(rows.begin(), rows.end(), before);
  return rows;
}

auto SOUND::VIEWS::WIRED::BROWSE::up(const String &filing) -> String {
  const Whole at = filing.rfind(WITHIN);
  return at == String::npos ? String() : filing.substr(0, at);
}

auto SOUND::VIEWS::WIRED::BROWSE::planted(
  const String &filing, const Vector<Offer> &plugins) -> Flag {
  if (filing.empty()) return true;
  const String root = ::step(filing, String());
  for (const Offer &standard : directories(String(), plugins))
    if (standard.among == root) return true;
  return false;
}

void SOUND::VIEWS::WIRED::BROWSE::head(GUI::Handle page, const String &filing) {
  String said = ::CATALOG;
  for (String rest = filing; !rest.empty(); rest = up(rest))
    said.insert(String(::CATALOG).size(), ::APART + ::last(rest));
  GUI::set(page, HEAD, GUI::Text{said});
  GUI::set(page, UP, GUI::Visibility{!filing.empty()});
}

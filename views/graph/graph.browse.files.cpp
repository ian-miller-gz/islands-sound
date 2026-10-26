// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../inventory.hpp"
#include "graph.browse.internal.hpp"

namespace {
using namespace SOUND;
namespace BROWSE = SOUND::VIEWS::WIRED::BROWSE;
using namespace BROWSE;

constexpr STRING::Hot TO = "→";
constexpr STRING::Hot AT = "@";
constexpr STRING::Hot SECONDS = "s";

String kept;
Flag fresh = false;
Vector<Offer> listing;

auto counted(const String &filing) -> Whole {
  const Folder held = BROWSE::read(filing);
  return held.folders.size() + held.waves.size();
}

auto reading(const INVENTORY::Heard &heard) -> String {
  if (heard.refusal[0] != '\0') return heard.refusal;
  const String rate = heard.rate == RATE
                        ? std::to_string(RATE)
                        : std::format("{} {} {}", heard.rate, ::TO, RATE);
  return std::format(
    "{} {} {} {:.1f} {}", heard.channels, ::AT, rate,
    Float(heard.frames) / Float(heard.rate), ::SECONDS);
}

auto listed(const String &filing) -> Vector<Offer> {
  const Folder held = BROWSE::read(filing);
  Vector<Offer> rows;
  for (const String &name : held.folders)
    rows.push_back(
      {.name = name,
       .from = FILES,
       .among = filing + WITHIN + name,
       .directory = true,
       .holds = ::counted(filing + WITHIN + name)});
  for (const String &name : held.waves) {
    const String path = held.path + name;
    rows.push_back(
      {.name = name,
       .from = FILES,
       .among = filing,
       .reading = ::reading(INVENTORY::heard(path)),
       .path = path});
  }
  return rows;
}

auto shelved() -> Offer {
  return {
    .name = FILES,
    .from = FILES,
    .among = FILES,
    .directory = true,
    .holds = ::counted(FILES)};
}

}  // namespace

auto SOUND::VIEWS::WIRED::BROWSE::files(const String &filing) -> Vector<Offer> {
  if (::fresh && filing == ::kept) return ::listing;
  ::kept = filing;
  ::fresh = true;
  ::listing.clear();
  if (filing.empty()) ::listing.push_back(::shelved());
  if (housed(filing)) ::listing = ::listed(filing);
  return ::listing;
}

auto SOUND::VIEWS::WIRED::BROWSE::housed(const String &filing) -> Flag {
  return filing == FILES || filing.starts_with(String(FILES) + WITHIN);
}

void SOUND::VIEWS::WIRED::BROWSE::forget() { ::fresh = false; }

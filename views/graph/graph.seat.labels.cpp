// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../boards.hpp"
#include "../../kind.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot LABELLED = "label";
constexpr STRING::Hot INKED = "slabink";
constexpr STRING::Hot CUT = "…";
constexpr Whole MARKED = 1;

}  // namespace

auto SOUND::VIEWS::WIRED::worded(const Port &worn) -> String {
  const String kind(KIND::spoken(worn.kind));
  if (worn.kind != KIND::DATA || worn.type.empty()) return kind;
  return std::format("{}.{}", kind, worn.type);
}

auto SOUND::VIEWS::WIRED::cropped(const String &word, Float wide, Float letter)
  -> String {
  const Whole columns = Whole(wide / (ADVANCE * letter));
  if (UNICODE::columns(word) <= columns) return word;
  if (columns <= ::MARKED) return {};
  String cut;
  Whole at = 0;
  while (UNICODE::columns(cut) + ::MARKED < columns)
    cut += UNICODE::encode(UNICODE::decode(word, at));
  return cut + String(::CUT);
}

void SOUND::VIEWS::WIRED::title(
  const String &box, const GUI::Position &at, const GUI::Extent &band,
  Float letter) {
  const GUI::Handle page = VIEWS::document();
  const String id = std::format("{}.{}", box, NAME);
  BOARDS::place(page, box.c_str(), "label", id, at, band);
  GUI::set(page, id.c_str(), GUI::Style{::INKED});
  GUI::set(page, id.c_str(), GUI::Size{letter});
}

void SOUND::VIEWS::WIRED::subtitle(
  const String &box, const GUI::Position &at, const GUI::Extent &band,
  Float letter) {
  const GUI::Handle page = VIEWS::document();
  const String id = std::format("{}.{}", box, DEVICE);
  BOARDS::place(page, box.c_str(), "label", id, at, band);
  GUI::set(page, id.c_str(), GUI::Style{::INKED});
  GUI::set(page, id.c_str(), GUI::Size{letter});
}

void SOUND::VIEWS::WIRED::label(
  const String &box, const String &socket, const GUI::Position &at,
  const GUI::Extent &size, Float letter, const Port &worn) {
  const GUI::Handle page = VIEWS::document();
  const String id = std::format("{}.{}", socket, ::LABELLED);
  const String said =
    worn.type.empty() ? worn.name : std::format("{}:{}", worn.name, worn.type);
  BOARDS::place(page, box.c_str(), "label", id, at, size);
  GUI::set(page, id.c_str(), GUI::Style{::INKED});
  GUI::set(page, id.c_str(), GUI::Size{letter});
  GUI::set(page, id.c_str(), GUI::Text{cropped(said, size.w, letter).c_str()});
}

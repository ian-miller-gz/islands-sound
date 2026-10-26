// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../boards.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

constexpr STRING::Hot LEAF = "reading";
constexpr STRING::Hot INK = "ink";
constexpr Float RAISED = 3.0f;
constexpr Float LETTER = 15.0f;
constexpr Float WIDE = 80.0f;
constexpr Float TALL = 20.0f;

VIEWS::Pool pool;

auto reading() -> String { return String(WAVE::HANDLE) + "." + ::LEAF; }

void build(GUI::Handle page, Whole count) {
  const String id = ::reading();
  if (count == 0 && ::pool.stood > 0) BOARDS::drop(page, id);
  if (count > 0 && ::pool.stood == 0) {
    BOARDS::place(page, WAVE::board().c_str(), WAVE::LABEL, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{::INK});
    GUI::set(page, id.c_str(), GUI::Depth{::RAISED});
  }
  VIEWS::built(::pool, count);
}

void written(GUI::Handle page, const WAVE::Clip &clip) {
  const String id = ::reading();
  const Float scale = WAVE::scaled();
  const GUI::Extent seat = {::WIDE / scale, ::TALL};
  const Float east = Float(clip.at + clip.frames) - seat.w;
  const Float north = WAVE::deep() - WAVE::handled().h - seat.h;
  GUI::set(page, id.c_str(), GUI::Position{east, north});
  GUI::set(page, id.c_str(), seat);
  GUI::set(page, id.c_str(), GUI::Size{::LETTER / scale});
  GUI::set(page, id.c_str(), GUI::Text{WAVE::worded(clip)});
}

}  // namespace

void SOUND::VIEWS::WAVE::SHED::reading() {
  VIEWS::reborn(::pool);
  ::build(document(), 0);
}

void SOUND::VIEWS::WAVE::say() {
  const GUI::Handle page = document();
  if (scaled() <= ORIGIN) return;
  const Vector<Clip> seated = placed();
  const Whole row = held();
  const Flag stands = row < seated.size();
  VIEWS::reborn(::pool);
  if (stands && ::pool.stood == 0) ::build(page, 1);
  if (::pool.stood == 0) return;
  GUI::set(page, ::reading().c_str(), GUI::Visibility{stands});
  if (stands) ::written(page, landed(row, seated[row]));
}

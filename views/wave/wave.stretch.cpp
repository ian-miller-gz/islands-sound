// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../boards.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

constexpr STRING::Hot KIND = "node";
constexpr Float RAISED = 2.0f;

VIEWS::Pool pool;

void build(GUI::Handle page, Whole count) {
  const String board = WAVE::board();
  for (Whole slot = count; slot < ::pool.stood; ++slot)
    BOARDS::drop(page, WAVE::handle(slot));
  for (Whole slot = ::pool.stood; slot < count; ++slot) {
    const String id = WAVE::handle(slot);
    BOARDS::place(page, board.c_str(), ::KIND, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{WAVE::GRIP});
    GUI::set(page, id.c_str(), GUI::Depth{::RAISED});
  }
  VIEWS::built(::pool, count);
}

void written(GUI::Handle page, Whole slot, const WAVE::Clip &clip) {
  const String id = WAVE::handle(slot);
  const Flag stands = WAVE::shown(clip);
  GUI::set(page, id.c_str(), GUI::Visibility{stands});
  if (!stands) return;
  const GUI::Extent seat = WAVE::handled();
  const Float east = Float(clip.at + clip.frames) - seat.w;
  GUI::set(page, id.c_str(), GUI::Position{east, WAVE::deep() - seat.h});
  GUI::set(page, id.c_str(), seat);
}

}  // namespace

auto SOUND::VIEWS::WAVE::handle(Whole row) -> String {
  return std::format("{}.{}", HANDLE, row);
}

auto SOUND::VIEWS::WAVE::handled() -> GUI::Extent {
  const GUI::Extent face = GUI::GET::extent(document(), HANDLE);
  return {face.w / scaled(), face.h};
}

void SOUND::VIEWS::WAVE::SHED::handles() {
  VIEWS::reborn(::pool);
  ::build(document(), 0);
}

void SOUND::VIEWS::WAVE::handles() {
  const GUI::Handle page = document();
  if (scaled() <= ORIGIN) return;
  const Vector<Clip> seated = placed();
  VIEWS::reborn(::pool);
  const Whole seats = VIEWS::sized(::pool, seated.size(), window());
  if (seats != ::pool.stood) ::build(page, seats);
  for (Whole slot = seated.size(); slot < ::pool.stood; ++slot)
    GUI::set(page, handle(slot).c_str(), GUI::Visibility{false});
  for (Whole slot = 0; slot < seated.size() && slot < ::pool.stood; ++slot)
    ::written(page, slot, landed(slot, seated[slot]));
}

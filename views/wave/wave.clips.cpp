// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

constexpr STRING::Hot LEAF = "zero";
constexpr STRING::Hot LINE = "wave clip {} at {} from {} frames {}";
constexpr STRING::Hot RATIO = " ratio {:g}";
constexpr STRING::Hot MARK = "selected";
constexpr Float RAISED = 2.0f;
constexpr Float CHOSEN = 3.0f;

VIEWS::Pool pool;

auto plate(Whole at) -> String {
  return GUI::SAC::SEAT::named(WAVE::board().c_str(), at);
}

auto zero(const String &plate) -> String { return plate + "." + ::LEAF; }

void build(GUI::Handle page, Whole count) {
  const String board = WAVE::board();
  for (Whole at = count; at < ::pool.stood; ++at)
    BOARDS::drop(page, ::plate(at));
  for (Whole at = ::pool.stood; at < count; ++at) {
    const String id = ::plate(at);
    BOARDS::place(page, board.c_str(), WAVE::PANEL, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{WAVE::PLATE});
    const String line = ::zero(id);
    GUI::NODES::create(page, id.c_str(), WAVE::PANEL, line.c_str());
    GUI::set(page, line.c_str(), GUI::Style{WAVE::ZERO});
    GUI::set(page, line.c_str(), GUI::Depth{::RAISED});
  }
  VIEWS::built(::pool, count);
}

auto stretched(const WAVE::Clip &clip) -> Flag {
  return clip.ratio != WAVE::Clip{}.ratio;
}

void dressed(GUI::Handle page, const String &id, Whole at, Flag stretched) {
  const GUI::Properties plain = GUI::GET::style(page, WAVE::PLATE);
  const Flag marked = at == WAVE::chosen();
  const GUI::Properties worn =
    stretched ? GUI::GET::style(page, WAVE::STRETCHED) : plain;
  GUI::set(page, id.c_str(), worn.color);
  GUI::set(
    page, id.c_str(),
    GUI::Edge{marked ? GUI::GET::style(page, ::MARK).press : plain.edge});
  GUI::set(page, id.c_str(), GUI::Border{marked ? ::CHOSEN : plain.border});
}

void written(GUI::Handle page, Whole at, const WAVE::Clip &clip, Float deep) {
  const String id = ::plate(at);
  const WAVE::Clip laid = WAVE::landed(at, clip);
  ::dressed(page, id, at, ::stretched(laid));
  const Float run = Float(laid.frames);
  GUI::set(page, id.c_str(), GUI::Position{Float(clip.at), WAVE::ORIGIN});
  GUI::set(page, id.c_str(), GUI::Extent{run, deep});
  const String line = ::zero(id);
  GUI::set(
    page, line.c_str(),
    GUI::Position{WAVE::ORIGIN, (deep - WAVE::HAIR) * WAVE::HALF});
  GUI::set(page, line.c_str(), GUI::Extent{run, WAVE::HAIR});
}

}  // namespace

void SOUND::VIEWS::WAVE::SHED::clips() {
  VIEWS::reborn(::pool);
  ::build(document(), 0);
}

void SOUND::VIEWS::WAVE::clips() {
  const GUI::Handle page = document();
  const Vector<Clip> seated = placed();
  VIEWS::reborn(::pool);
  const Whole seats = VIEWS::sized(::pool, seated.size(), window());
  if (seats != ::pool.stood) ::build(page, seats);
  VIEWS::light(::pool, ::plate, seated.size());
  const Float down = deep();
  for (Whole at = 0; at < seated.size(); ++at)
    ::written(page, at, seated[at], down);
}

void SOUND::VIEWS::WAVE::clips(SHELL::Session &session) {
  const Vector<Clip> seated = placed();
  for (Whole at = 0; at < seated.size(); ++at)
    session.print(
      std::format(
        ::LINE, at, seated[at].at, seated[at].from, seated[at].frames) +
      (::stretched(seated[at]) ? std::format(::RATIO, seated[at].ratio) : ""));
}

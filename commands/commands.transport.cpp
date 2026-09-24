// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../render.hpp"
#include "../score.hpp"
#include "../transport.hpp"
#include "../views.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot RUNNING = "on";
constexpr STRING::Hot RESTING = "off";

auto stated() -> String {
  const TRANSPORT::Marker marker = TRANSPORT::marker();
  const Setting &setting = TRANSPORT::held();
  const Whole at = SCORE::pulsed(marker.position, setting.tempos);
  const Metre metre = TRANSPORT::metred(at);
  const Span span = TRANSPORT::cycled();
  const String cycle = setting.looping && span.to > span.from
                         ? std::format("{} {}", span.from, span.to)
                         : String(::RESTING);
  return std::format(
    "transport {} position {} frames {:.3f} s tempo {:.3f} {}/{} loop {}",
    marker.playing ? "playing" : "stopped", marker.position,
    TRANSPORT::seconds(marker.position), TRANSPORT::paced(at), metre.numerator,
    metre.denominator, cycle);
}

void listed(SHELL::Session &session) {
  const Vector<Tempo> &tempos = TRANSPORT::held().tempos;
  if (tempos.size() < 2) return;
  for (const Tempo &row : tempos)
    session.print(std::format("tempo {:.3f} at {}", row.tempo, row.at));
}

}  // namespace

void SOUND::COMMANDS::timing(SHELL::Session &session) {
  session.print(::stated());
}

void SOUND::COMMANDS::play(SHELL::Session &session) {
  RENDER::spool();
  TRANSPORT::play();
  session.print(::stated());
}

void SOUND::COMMANDS::stop(SHELL::Session &session) {
  TRANSPORT::stop();
  RENDER::settle();
  session.print(::stated());
}

void SOUND::COMMANDS::locate(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("locate <frame>");
  TRANSPORT::locate(std::strtoul(session.arguments[1].c_str(), nullptr, 10));
  session.print(::stated());
}

void SOUND::COMMANDS::chase(SHELL::Session &session) {
  if (session.arguments.size() > 1)
    VIEWS::chasing(session.arguments[1] == ::RUNNING);
  session.print(std::format(
    "chase {} head {}", VIEWS::chasing() ? ::RUNNING : ::RESTING,
    VIEWS::seen() ? "in" : "out"));
}

void SOUND::COMMANDS::tempo(SHELL::Session &session) {
  const Asked ask = asked(session);
  if (ask.off && !paced(ask.at))
    return session.print(std::format("tempo refused: no row at {}", ask.at));
  if (ask.words > 1 && !ask.off)
    paced(ask.at, std::strtof(session.arguments[1].c_str(), nullptr));
  session.print(::stated());
  ::listed(session);
}

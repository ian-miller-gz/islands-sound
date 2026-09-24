// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../timeline.hpp"
#include "../../transport.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot LIST = "tracks";

}  // namespace

auto SOUND::VIEWS::SIGNALS::attended() -> Whole {
  const Whole track = VIEWS::cursor(::LIST);
  return track < TIMELINE::held().tracks.size() ? track : NONE;
}

auto SOUND::VIEWS::SIGNALS::spans() -> Vector<Span> {
  Vector<Span> lit;
  for (const ARRANGEMENT::TRACK::LANE::Clip &span :
       TIMELINE::spans(attended(), paged()))
    lit.push_back({span.stock, span.at, span.at + span.frames});
  return lit;
}

auto SOUND::VIEWS::SIGNALS::aimed() -> Aim {
  return aimed(TRANSPORT::marker().position);
}

auto SOUND::VIEWS::SIGNALS::aimed(Whole at) -> Aim {
  const ARRANGEMENT::TRACK::LANE::Clip span =
    TIMELINE::covered(attended(), paged(), at);
  if (span.stock == NONE) return {};
  return {span.stock, span.from, span.at};
}

void SOUND::VIEWS::SIGNALS::aimed(SHELL::Session &session) {
  const Aim aim = aimed();
  if (aim.stock == NONE) return session.print("aims nothing");
  session.print(std::format(
    "aims stock {} from {} opens {}", aim.stock, aim.from, aim.opens));
}

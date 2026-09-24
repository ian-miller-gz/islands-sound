// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../transport.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot RUNNING = "on";
constexpr STRING::Hot RESTING = "off";
constexpr STRING::Hot OPENED = "cycle.from";
constexpr STRING::Hot CLOSED = "cycle.to";
constexpr STRING::Hot POSTED = "tag";
constexpr STRING::Hot HEADED = "ends.from";
constexpr STRING::Hot TAILED = "ends.to";
constexpr Whole ONLY = 0;

void flag(
  SHELL::Session &session, STRING::Hot kind, Whole at, Whole frames, Float west,
  Float scale, VIEWS::Fold fold) {
  session.print(
    std::format("flag {} {} {:g}", kind, at, fold(Float(frames), west, scale)));
}

}  // namespace

void SOUND::VIEWS::flagged(
  SHELL::Session &session, Whole cells, Float west, Float scale, Fold fold) {
  const Setting &setting = TRANSPORT::held();
  const Flag ends = setting.ends.to > setting.ends.from;
  session.print(std::format(
    "cycle loops {} tags {} ends {} looping {} cells {}", setting.loops.size(),
    setting.tags.size(), ends ? ::RUNNING : ::RESTING,
    setting.looping ? ::RUNNING : ::RESTING, cells));
  for (Whole at = 0; at < setting.loops.size(); ++at) {
    ::flag(
      session, ::OPENED, at, framed(setting.loops[at].from), west, scale, fold);
    ::flag(
      session, ::CLOSED, at, framed(setting.loops[at].to), west, scale, fold);
  }
  for (Whole at = 0; at < setting.tags.size(); ++at)
    ::flag(
      session, ::POSTED, at, framed(setting.tags[at].at), west, scale, fold);
  if (!ends) return;
  ::flag(
    session, ::HEADED, ::ONLY, framed(setting.ends.from), west, scale, fold);
  ::flag(session, ::TAILED, ::ONLY, framed(setting.ends.to), west, scale, fold);
}

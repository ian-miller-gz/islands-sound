// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "session.store.span.hpp"

using namespace SOUND;
using Clip = ARRANGEMENT::TRACK::LANE::Clip;
using Fade = ARRANGEMENT::TRACK::LANE::Fade;

namespace {

auto neutral(const Fade &fade) -> Flag {
  const Fade blank;
  return fade.frames == blank.frames && fade.shape == blank.shape;
}

auto neutral(const Clip &span) -> Flag {
  const Clip blank;
  return ::neutral(span.in) && ::neutral(span.out) &&
         span.ratio == blank.ratio && span.cents == blank.cents;
}

template <typename Field>
void trailing(std::istringstream &line, Field &field) {
  Field token = field;
  if (line >> token) field = token;
}

void trailing(std::istringstream &line, Fade &fade) {
  ::trailing(line, fade.frames);
  ::trailing(line, fade.shape);
}

}  // namespace

auto SOUND::SESSION::spanned(const Clip &span) -> String {
  String text =
    std::format("{} {} {} {}", span.stock, span.at, span.from, span.frames);
  if (::neutral(span)) return text;
  return text + std::format(
                  " {} {} {} {} {:g} {}", span.in.frames, span.in.shape,
                  span.out.frames, span.out.shape, span.ratio, span.cents);
}

auto SOUND::SESSION::spanned(std::istringstream &line) -> Clip {
  Clip span;
  line >> span.stock >> span.at >> span.from >> span.frames;
  ::trailing(line, span.in);
  ::trailing(line, span.out);
  ::trailing(line, span.ratio);
  ::trailing(line, span.cents);
  return span;
}

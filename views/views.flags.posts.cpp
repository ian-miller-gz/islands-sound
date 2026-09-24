// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../transport.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

auto numbered(const String &strip, STRING::Hot word, Whole at) -> String {
  return std::format("{}.{}.{}", strip, word, at);
}

void spanned(
  Vector<VIEWS::Post> &standing, VIEWS::Post::Kind kind, Whole at,
  const Span &span, const String &stem) {
  standing.push_back({kind, at, span.from, false, stem + "." + VIEWS::OPENED});
  standing.push_back({kind, at, span.to, true, stem + "." + VIEWS::CLOSED});
}

}  // namespace

auto SOUND::VIEWS::label(const Post &post) -> String {
  if (post.kind == Post::BOUND) return {};
  const String stem =
    post.kind == Post::TAGGED ? post.id : post.id.substr(0, post.id.rfind('.'));
  return stem + "." + LABEL;
}

auto SOUND::VIEWS::posts(const String &strip) -> Vector<Post> {
  const Setting &setting = TRANSPORT::held();
  Vector<Post> standing;
  for (Whole at = 0; at < setting.loops.size(); ++at)
    ::spanned(
      standing, Post::CYCLED, at, setting.loops[at],
      ::numbered(strip, CYCLE, at));
  for (Whole at = 0; at < setting.tags.size(); ++at)
    standing.push_back(
      {Post::TAGGED, at, setting.tags[at].at, false,
       ::numbered(strip, TAG, at)});
  if (setting.ends.to > setting.ends.from)
    ::spanned(
      standing, Post::BOUND, 0, setting.ends, strip + "." + String(ENDS));
  return standing;
}

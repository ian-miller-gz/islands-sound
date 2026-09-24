// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../commands.hpp"
#include "../transport.hpp"
#include "views.flags.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ERASES = "Delete";
constexpr STRING::Hot EDITS = "Edit";
constexpr STRING::Hot WHOSE = "signposts";
constexpr Whole ERASE = 0;
constexpr Whole NOWHERE = 0;

VIEWS::Post about;

void erased(const VIEWS::Post &post) {
  const Setting &setting = TRANSPORT::held();
  if (post.kind == VIEWS::Post::TAGGED)
    return void(COMMANDS::flagged(setting.tags[post.at].at));
  if (post.kind == VIEWS::Post::BOUND)
    return void(COMMANDS::ended(::NOWHERE, ::NOWHERE));
  COMMANDS::looped(setting.loops[post.at].from);
}

void took(Whole row) {
  const VIEWS::Post held = ::about;
  ::about = {};
  if (row == ::ERASE) return ::erased(held);
  const String cell = VIEWS::label(held);
  GUI::set(VIEWS::document(), cell.c_str(), GUI::Visibility{true});
  GUI::edit(VIEWS::document(), cell.c_str());
}

auto flagged(const Vector<VIEWS::Post> &standing, const GUI::NGA::Ask &ask)
  -> const VIEWS::Post * {
  if (!ask.asked || !ask.board.empty()) return nullptr;
  for (const VIEWS::Post &post : standing) {
    if (ask.target.compare(0, post.id.size(), post.id) != 0) continue;
    if (
      ask.target.size() == post.id.size() || ask.target[post.id.size()] == '.')
      return &post;
  }
  return nullptr;
}

void raise(const VIEWS::Post &post, Float x, Float y) {
  Vector<String> rows{String(::ERASES)};
  if (post.kind != VIEWS::Post::BOUND) rows.push_back(String(::EDITS));
  ::about = post;
  VIEWS::carried().clear();
  VIEWS::MENU::raise(rows, x, y, ::WHOSE);
}

}  // namespace

void SOUND::VIEWS::offered(const Vector<Post> &standing) {
  if (!::about.id.empty()) {
    const Whole row = MENU::taken(::WHOSE);
    if (row != NONE) return ::took(row);
    if (!MENU::standing()) ::about = {};
  }
  const GUI::NGA::Ask ask = GUI::NGA::GET::asked(document());
  const Post *post = ::flagged(standing, ask);
  if (post != nullptr) ::raise(*post, ask.x, ask.y);
}

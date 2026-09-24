// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../commands.hpp"
#include "../history.hpp"
#include "../transport.hpp"
#include "views.flags.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

String carrying;
String seating;
String dragging;
Whole born = 0;

void arced(const String &strip, Flag moving) {
  if (moving && ::dragging.empty()) {
    ::dragging = strip;
    HISTORY::begin();
  } else if (!moving && ::dragging == strip) {
    ::dragging.clear();
    HISTORY::end();
  }
}

void tagged(const VIEWS::Post &post, Whole place) {
  const Tag held = TRANSPORT::held().tags[post.at];
  COMMANDS::flagged(held.at);
  COMMANDS::flagged(place, held.text);
}

void looped(const VIEWS::Post &post, Whole place) {
  const Span held = TRANSPORT::held().loops[post.at];
  const Whole from = post.closing ? held.from : place;
  const Whole to = post.closing ? place : held.to;
  COMMANDS::looped(held.from);
  if (!COMMANDS::looped(from, to, held.text))
    COMMANDS::looped(held.from, held.to, held.text);
}

void bound(const VIEWS::Post &post, Whole place) {
  const Span held = TRANSPORT::held().ends;
  COMMANDS::ended(
    post.closing ? held.from : place, post.closing ? place : held.to);
}

void moved(const VIEWS::Post &post, Whole place) {
  if (post.kind == VIEWS::Post::TAGGED) return ::tagged(post, place);
  if (post.kind == VIEWS::Post::BOUND) return ::bound(post, place);
  ::looped(post, place);
}

auto landed(
  const String &strip, const VIEWS::Post &post, Float west, Float scale,
  VIEWS::Unfold fold) -> Whole {
  return fold(VIEWS::planted(strip, post.id, post.closing), west, scale);
}

void pressing(const Vector<VIEWS::Post> &standing) {
  const GUI::Handle page = VIEWS::document();
  if (!GUI::GET::pressed(page)) return;
  VIEWS::carried().clear();
  for (const GUI::Event &event : GUI::GET::events(page))
    for (const VIEWS::Post &post : standing)
      if (event.kind == GUI::Event::PRESSED && post.id == String(event.id))
        VIEWS::carried() = post.id;
}

auto words(const VIEWS::Post &post) -> const String & {
  const Setting &setting = TRANSPORT::held();
  return post.kind == VIEWS::Post::TAGGED ? setting.tags[post.at].text
                                          : setting.loops[post.at].text;
}

void named(const VIEWS::Post &post, const String &text) {
  if (post.kind == VIEWS::Post::TAGGED)
    return void(COMMANDS::flagged(post.place, text));
  const Span held = TRANSPORT::held().loops[post.at];
  COMMANDS::looped(held.from, held.to, text);
}

void worded(const VIEWS::Post &post) {
  const GUI::Handle page = VIEWS::document();
  const String cell = VIEWS::label(post);
  if (cell.empty() || post.closing) return;
  if (GUI::GET::committed(page, cell.c_str())) {
    ::named(post, String(GUI::GET::text(page, cell.c_str())));
    return void(GUI::edit(page, ""));
  }
  if (String(GUI::GET::editing(page)) != cell || ::seating == cell) return;
  GUI::set(
    page, cell.c_str(), GUI::Caret{::words(post).size(), ::words(post).size()});
}

}  // namespace

auto SOUND::VIEWS::carried() -> String & { return ::carrying; }

void SOUND::VIEWS::handled(
  const String &strip, Float west, Float scale, Unfold unfold) {
  const GUI::Handle page = document();
  if (reborn(::born)) ::carrying.clear();
  const Vector<Post> standing = posts(strip);
  const String editing = String(GUI::GET::editing(page));
  ::pressing(standing);
  Whole taken = NONE;
  for (Whole at = 0; at < standing.size(); ++at) {
    ::worded(standing[at]);
    if (taken == NONE && GUI::GET::moved(page, standing[at].id.c_str()))
      taken = at;
  }
  ::seating = editing;
  ::arced(strip, taken != NONE);
  if (taken == NONE) return offered(standing);
  ::carrying.clear();
  ::moved(
    standing[taken], ::landed(strip, standing[taken], west, scale, unfold));
}

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../views.kept.hpp"
#include "lane.internal.hpp"

namespace {
using namespace SOUND;
namespace LANE = SOUND::VIEWS::LANE;
namespace KEPT = SOUND::VIEWS::KEPT;

constexpr STRING::Hot FACES = "lane.faces";
constexpr STRING::Hot MAPPED = "mapped";

template <typename Type>
struct Row {
  STRING::Hot leaf = "";
  Type &(*field)(LANE::Face &) = nullptr;
};

constexpr Row<String> WORDS[] = {
  {"track", [](LANE::Face &one) -> String & { return one.track; }},
  {"lane", [](LANE::Face &one) -> String & { return one.lane; }}};

constexpr Row<Float> REALS[] = {
  {"x", [](LANE::Face &one) -> Float & { return one.pan.x; }},
  {"y", [](LANE::Face &one) -> Float & { return one.pan.y; }},
  {"across", [](LANE::Face &one) -> Float & { return one.zoom.value; }},
  {"down", [](LANE::Face &one) -> Float & { return one.zoom.down; }}};

constexpr Row<Whole> COUNTS[] = {
  {"number", [](LANE::Face &one) -> Whole & { return one.number; }},
  {"notation", [](LANE::Face &one) -> Whole & { return one.notation; }}};

auto read(const FIELDS::Map &face, const String &key, String &value) -> Flag {
  return KEPT::text(face, key, value);
}

template <typename Type>
auto read(const FIELDS::Map &face, const String &key, Type &value) -> Flag {
  return KEPT::number(face, key, value);
}

template <typename Type, Whole Size>
void write(
  FIELDS::Map &face, Whole n, LANE::Face &one, const Row<Type> (&rows)[Size]) {
  for (const Row<Type> &row : rows)
    face[KEPT::join(::FACES, n, row.leaf)] = row.field(one);
}

template <typename Type, Whole Size>
void read(
  const FIELDS::Map &face, Whole n, LANE::Face &one,
  const Row<Type> (&rows)[Size]) {
  for (const Row<Type> &row : rows)
    ::read(face, KEPT::join(::FACES, n, row.leaf), row.field(one));
}

}  // namespace

void SOUND::VIEWS::LANE::keep(FIELDS::Map &face, Whole n, Face &one) {
  ::write(face, n, one, ::WORDS);
  ::write(face, n, one, ::REALS);
  ::write(face, n, one, ::COUNTS);
  const String mapped = KEPT::join(::FACES, n, ::MAPPED);
  face[mapped] = one.mapped.size();
  for (Whole m = 0; m < one.mapped.size(); ++m)
    face[KEPT::join(mapped, m)] = one.mapped[m];
}

void SOUND::VIEWS::LANE::restore(const FIELDS::Map &face, Whole n, Face &one) {
  ::read(face, n, one, ::WORDS);
  ::read(face, n, one, ::REALS);
  ::read(face, n, one, ::COUNTS);
  const String mapped = KEPT::join(::FACES, n, ::MAPPED);
  Whole count = 0;
  KEPT::number(face, mapped, count);
  for (Whole m = 0; m < count; ++m) {
    String name;
    if (KEPT::text(face, KEPT::join(mapped, m), name))
      one.mapped.push_back(name);
  }
}

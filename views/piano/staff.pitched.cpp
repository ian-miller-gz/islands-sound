// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdlib>
#include <format>
#include <tuple>

#include "../../score.hpp"
#include "piano.internal.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

constexpr Whole LETTERS = 7;
constexpr Whole NATURAL[LETTERS] = {0, 2, 4, 5, 7, 9, 11};
constexpr STRING::Hot NAMES[LETTERS] = {"C", "D", "E", "F", "G", "A", "B"};
constexpr STRING::Hot SPELT[] = {"", "#", "b", "", "##", "bb"};
constexpr Integer FAR = 100, OCTAVE = 1;

struct Spelling {
  Whole letter = 0;
  Integer accidental = 0;
};

struct Signature {
  Vector<Spelling> degrees;
  Vector<Whole> classes;
  Integer worn[LETTERS] = {};
  Flag tonal = false;
};

auto lettered(Whole letter, Whole pitch) -> Integer {
  Integer off = Integer(pitch % CLASSES) - Integer(NATURAL[letter]);
  if (off > CLASSES / 2) off -= CLASSES;
  if (off < -Integer(CLASSES / 2)) off += CLASSES;
  return off;
}

auto spelt(const Vector<Whole> &classes, Whole tonic) -> Vector<Spelling> {
  Vector<Spelling> best;
  Integer least = ::FAR;
  for (Whole letter = 0; letter < LETTERS; ++letter) {
    if (std::abs(::lettered(letter, tonic)) > OCTAVE) continue;
    Vector<Spelling> degrees;
    Integer cost = 0;
    for (Whole degree = 0; degree < classes.size(); ++degree) {
      const Whole at = (letter + degree) % LETTERS;
      degrees.push_back({at, ::lettered(at, classes[degree])});
      cost += std::abs(degrees.back().accidental);
    }
    if (cost < least) std::tie(best, least) = std::tie(degrees, cost);
  }
  return best;
}

auto signed_(const Key &key) -> Signature {
  Signature signature;
  Vector<Whole> classes(LETTERS, NONE);
  Whole counted = 0;
  for (Whole away = 0; away < CLASSES; ++away) {
    const Whole degree = SCORE::degree(key, key.tonic % CLASSES + away);
    if (degree == NONE || degree > LETTERS) continue;
    classes[degree - 1] = (key.tonic + away) % CLASSES;
    ++counted;
  }
  if (counted != LETTERS) return signature;
  signature.classes = classes;
  signature.degrees = ::spelt(classes, key.tonic % CLASSES);
  signature.tonal = !signature.degrees.empty();
  for (const Spelling &degree : signature.degrees)
    signature.worn[degree.letter] = degree.accidental;
  return signature;
}

auto spelled(const Signature &signature, Whole pitch) -> Spelling {
  const Whole klass = pitch % CLASSES;
  if (signature.tonal)
    for (Whole degree = 0; degree < LETTERS; ++degree)
      if (signature.classes[degree] == klass) return signature.degrees[degree];
  Integer leaning = 0;
  for (const Integer worn : signature.worn) leaning += worn;
  const Integer ways[] = {leaning >= 0 ? -1 : 1, leaning >= 0 ? 1 : -1};
  for (const Integer way : ways) {
    const Whole near = (klass + CLASSES + way) % CLASSES;
    for (Whole degree = 0; signature.tonal && degree < LETTERS; ++degree) {
      if (signature.classes[degree] != near) continue;
      const Spelling &base = signature.degrees[degree];
      if (std::abs(base.accidental - way) <= OCTAVE)
        return {base.letter, base.accidental - way};
    }
  }
  for (Whole letter = 0; letter < LETTERS; ++letter)
    if (NATURAL[letter] == klass) return {letter, 0};
  for (Whole letter = 0; letter < LETTERS; ++letter)
    if (NATURAL[letter] + 1 == klass) return {letter, 1};
  return {};
}

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::spelled(const Key &key, Whole pitch) -> Spelt {
  const Signature signature = ::signed_(key);
  const Spelling spelling = ::spelled(signature, pitch);
  const Integer natural = Integer(pitch) - spelling.accidental;
  const Integer octave = (natural + Integer(CLASSES)) / Integer(CLASSES) - 2;
  const Integer diatonic = octave * Integer(LETTERS) + Integer(spelling.letter);
  const Whole said = spelling.accidental == 0    ? 0
                     : spelling.accidental == 1  ? 1
                     : spelling.accidental == -1 ? 2
                     : spelling.accidental == 2  ? 4
                                                 : 5;
  const Whole signs[] = {SIGN::NATURAL, SIGN::SHARP, SIGN::FLAT,
                         SIGN::NATURAL, SIGN::DOUBLE_SHARP, SIGN::DOUBLE_FLAT};
  const Flag worn = spelling.accidental == signature.worn[spelling.letter];
  return {
    REFERENCE - diatonic, worn ? NONE : signs[said],
    std::format("{}{}{}", NAMES[spelling.letter], SPELT[said], octave)};
}

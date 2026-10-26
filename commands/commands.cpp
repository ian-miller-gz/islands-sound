// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../commands.hpp"
#include "../render.hpp"
#include "../views.hpp"
#include "commands.offer.hpp"

namespace {
using namespace SOUND;

const Vector<SHELL::Command> rows = {
  {"session", "", COMMANDS::document},
  {"new", "", COMMANDS::fresh},
  {"save", "", COMMANDS::save},
  {"open", "", COMMANDS::open},
  {"sessions", "", COMMANDS::saved},
  {"husks", "", COMMANDS::husks},
  {"time", "", COMMANDS::timing},
  {"play", "", COMMANDS::play},
  {"stop", "", COMMANDS::stop},
  {"locate", "", COMMANDS::locate},
  {"chase", "", COMMANDS::chase},
  {"settings", "", COMMANDS::settings},
  {"tempo",
   ""
   "[at <pulses>] | tempo off at <pulses>",
   COMMANDS::tempo},
  {"metre",
   ""
   "[at <pulses>] | metre off at <pulses>",
   COMMANDS::metre},
  {"loop",
   ""
   "<text ...>, lists the loops, or turns the wrap: loop on|off",
   COMMANDS::cycle},
  {"unloop", "", COMMANDS::unloop},
  {"tag", "",
   COMMANDS::tag},
  {"untag", "", COMMANDS::untag},
  {"ends", "",
   COMMANDS::ends},
  {"ahead", "", COMMANDS::ahead},
  {"behind", "", COMMANDS::behind},
  {"scale",
   ""
   "[at <pulses>] | scale off [at <pulses>]",
   COMMANDS::scale},
  {"plugins", "", COMMANDS::offers},
  {"seat", "",
   COMMANDS::seat},
  {"surface",
   ""
   "surface <track> <input|output|midiout> <device ...>",
   COMMANDS::surface},
  {"io", "", COMMANDS::io},
  {"main", "",
   COMMANDS::main},
  {"pair",
   ""
   "pair <node> <first lane>",
   COMMANDS::pair},
  {"root", "", COMMANDS::root},
  {"wire", "", COMMANDS::wire},
  {"unwire", "", COMMANDS::unwire},
  {"quiet", "",
   COMMANDS::quiet},
  {"bypass",
   ""
   "bypass <root> <out> [on|off]",
   COMMANDS::bypass},
  {"unseat", "",
   COMMANDS::unseat},
  {"home", "", COMMANDS::home},
  {"graph", "", COMMANDS::wiring},
  {"peak", "",
   COMMANDS::peak},
  {"note", "", COMMANDS::note},
  {"turn", "", COMMANDS::turn},
};

}  // namespace

auto SOUND::COMMANDS::table() -> const Vector<SHELL::Command>& {
  static const Vector<SHELL::Command> table = [] {
    Vector<SHELL::Command> words = ::rows;
    for (const Vector<SHELL::Command>& more :
         {later(), offered(), RENDER::words()})
      words.insert(words.end(), more.begin(), more.end());
    for (const VIEWS::View& unit : VIEWS::roster())
      words.insert(words.end(), unit.words.begin(), unit.words.end());
    return words;
  }();
  return table;
}

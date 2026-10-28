// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../commands.hpp"
#include "commands.offer.hpp"

namespace {
using namespace SOUND;

const Vector<SHELL::Command> rows = {
  {"bounce", "",
   COMMANDS::bounce},
  {"timeline", "", COMMANDS::tracks},
  {"track", "", COMMANDS::track},
  {"bus", "",
   COMMANDS::bus},
  {"name", "",
   COMMANDS::name},
  {"drop", "", COMMANDS::drop},
  {"lane", "", COMMANDS::lane},
  {"unlane", "", COMMANDS::unlane},
  {"kind",
   ""
   "<kind>",
   COMMANDS::kind},
  {"admit",
   ""
   "admit <track> <lane> [on|off]",
   COMMANDS::admit},
  {"clip", "", COMMANDS::place},
  {"span", "", COMMANDS::span},
  {"export",
   ""
   "and writes it as a wav: export <track> [path]",
   COMMANDS::exporting},
  {"join",
   ""
   "clip, gaps empty",
   COMMANDS::join},
  {"lift", "", COMMANDS::lift},
  {"map",
   ""
   "map <track> <lane> [plugin|none]",
   COMMANDS::map},
  {"restore", "",
   COMMANDS::restore},
  {"stocks", "", COMMANDS::stocks},
  {"stock", "", COMMANDS::stock},
  {"length", "states a stock's length", COMMANDS::length},
  {"author", "", COMMANDS::author},
  {"move", "",
   COMMANDS::move},
  {"stretch", "", COMMANDS::stretch},
  {"erase", "", COMMANDS::erase},
  {"take", "",
   COMMANDS::take},
  {"aim", "", COMMANDS::aim},
  {"plot", "",
   COMMANDS::plot},
  {"dial", "", COMMANDS::dial},
  {"sounding",
   ""
   "document's standing values",
   COMMANDS::sounding},
  {"history", "", COMMANDS::history},
  {"undo", "", COMMANDS::undo},
  {"redo", "", COMMANDS::redo},
  {"trim", "", COMMANDS::trim},
  {"roll", "",
   COMMANDS::roll},
  {"drive", "", COMMANDS::drive},
  {"strike", "", COMMANDS::strike},
  {"hear", "", COMMANDS::hear},
  {"record", "", COMMANDS::record},
  {"beat",
   ""
   "times: beat [n]",
   COMMANDS::beat},
  {"input",
   ""
   "one: input [<device> [<channel>]]",
   COMMANDS::input},
  {"views", "", COMMANDS::views},
  {"view", "",
   COMMANDS::view},
  {"panel", "", COMMANDS::panel},
  {"zoom", "", COMMANDS::zoom},
  {"pan", "", COMMANDS::pan},
  {"look", "",
   COMMANDS::look},
  {"checkpoint", "", COMMANDS::checkpoint},
  {"recovery", "", COMMANDS::recovery},
  {"recover", "", COMMANDS::recover}};

}  // namespace

auto SOUND::COMMANDS::later() -> Vector<SHELL::Command> { return ::rows; }

// SPDX-License-Identifier: AGPL-3.0-or-later
#include "session.store.internal.hpp"

using namespace SOUND;

namespace {

constexpr STRING::Hot OPENS = "x";
constexpr STRING::Hot PARTS = "xu";

struct Spool {
  String facts;
  Vector<Whole> where;
  Flag parting = false;
};

void borrowed(
  const String &facts, const Vector<Whole> &where, Graph &wiring,
  Vector<Husk> &husks) {
  const Whole from = husks.size();
  SESSION::taken(facts, wiring, husks);
  for (Whole one = from; one < husks.size(); ++one)
    if (husks[one].at < where.size()) husks[one].at = where[husks[one].at];
}

void spooled(Spool &spool, Edit &edit, Vector<Husk> &husks) {
  Graph read;
  ::borrowed(
    spool.facts, spool.where, spool.parting ? read : edit.graph, husks);
  Vector<Node> &nodes = edit.parted.nodes;
  nodes.insert(nodes.end(), read.nodes.begin(), read.nodes.end());
  spool = {};
}

auto opened(std::istringstream &line) -> Edit {
  Edit edit;
  Whole verb = Edit::ADDED;
  line >> edit.arc >> verb;
  edit.verb = verb;
  edit.act = SESSION::rest(line);
  return edit;
}

}  // namespace

void SOUND::SESSION::taken(
  const String &text, History &history, Vector<Husk> &husks) {
  ::Spool spool;
  std::istringstream lines(text);
  Whole at = 0;
  for (String held; std::getline(lines, held); ++at) {
    std::istringstream line(held);
    String tag;
    line >> tag;
    if (tag == ::OPENS || tag == ::PARTS) {
      if (!history.edits.empty()) ::spooled(spool, history.edits.back(), husks);
      spool.parting = tag == ::PARTS;
      if (tag == ::OPENS) history.edits.push_back(::opened(line));
    } else if (history.edits.empty()) {
      continue;
    } else if (!edited(tag, line, history.edits.back())) {
      spool.facts += held + "\n";
      spool.where.push_back(at);
    }
  }
  if (!history.edits.empty()) ::spooled(spool, history.edits.back(), husks);
}

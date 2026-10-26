// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <deque>

#include "graph.internal.hpp"

namespace {
using namespace SOUND;

using Pool = std::deque<GRAPH::Stand>;

struct Retiree {
  GRAPH::Stand *stand = nullptr;
  Whole stamp = 0;
};

Pool pool;
Vector<GRAPH::Stand *> spares;
Vector<Retiree> retirees;

void destroyed(GRAPH::Stand &stand) {
  if (stand.offer != nullptr && stand.instance != nullptr)
    stand.offer->surface->destroy(stand.instance);
  stand = {};
  ::spares.push_back(&stand);
}

}  // namespace

auto SOUND::GRAPH::slot(const Stand &stand) -> Stand & {
  if (::spares.empty()) return ::pool.emplace_back(stand);
  Stand &spare = *::spares.back();
  ::spares.pop_back();
  spare = stand;
  return spare;
}

void SOUND::GRAPH::retire(Stand &stand) {
  ::retirees.push_back({.stand = &stand, .stamp = PLAN::written()});
}

void SOUND::GRAPH::reap(Whole adopted) {
  std::erase_if(::retirees, [&](const Retiree &retiree) {
    if (retiree.stamp >= adopted) return false;
    ::destroyed(*retiree.stand);
    return true;
  });
}

void SOUND::GRAPH::reap() {
  for (const Retiree &retiree : ::retirees) ::destroyed(*retiree.stand);
  ::retirees.clear();
}

auto SOUND::GRAPH::retired() -> Whole { return ::retirees.size(); }

void SOUND::GRAPH::rest() {
  for (Whole at = 0; at < held().nodes.size(); ++at) {
    Stand &half = stand(at);
    if (half.offer == nullptr) continue;
    if (half.instance != nullptr) half.offer->surface->destroy(half.instance);
    half.instance = half.offer->surface->create(RATE, CHANNELS);
  }
  rebind();
}

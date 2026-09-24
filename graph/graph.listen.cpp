// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

void SOUND::GRAPH::listen() {
  for (const Stand &stand : stands()) {
    if (stand.offer == nullptr || stand.instance == nullptr) continue;
    if (stand.offer->listen != nullptr) stand.offer->listen(stand.instance);
  }
}

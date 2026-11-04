// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "graph.internal.hpp"

namespace SOUND::VIEWS::WIRED {

void keys();
void deleted(GUI::Handle page, const INPUT::KEYS::Event &key);
auto chosen(GUI::Handle page) -> Vector<String>;

}  // namespace SOUND::VIEWS::WIRED

namespace SOUND::VIEWS::WIRED::BROWSE {

void enter(const String &path);
void quick(Flag on);
auto quicked() -> Flag;
void typed(GUI::Handle page, const INPUT::KEYS::Event &key);

}  // namespace SOUND::VIEWS::WIRED::BROWSE

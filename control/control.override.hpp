// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "control.hpp"

namespace SOUND::CONTROL {

void overrule(const Address &address);
auto overridden(const Address &address) -> Flag;
auto overrides() -> Vector<Address>;
void restore(const Address &address);
void restore();
auto shifted() -> Vector<String>;

}  // namespace SOUND::CONTROL

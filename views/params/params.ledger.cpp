// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../commands.hpp"
#include "../../history.hpp"
#include "params.internal.hpp"

namespace {
using namespace SOUND;

Flag arcing = false;
Flag stirred = false;

}  // namespace

void SOUND::VIEWS::RACK::dial(const Row &row, Float value) {
  if (!::arcing) HISTORY::begin();
  ::arcing = true;
  ::stirred = true;
  COMMANDS::dialled(row.node, row.parameter, value);
  COMMANDS::overrule(row.node, row.parameter);
}

void SOUND::VIEWS::RACK::settle() {
  if (::arcing && !::stirred) {
    HISTORY::end();
    ::arcing = false;
  }
  ::stirred = false;
}

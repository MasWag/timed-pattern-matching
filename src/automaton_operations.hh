#pragma once

#include "types.hh"

void reverseNFA (const NFA &in, NFA &out);
void removeUnreachableStates (const RegionAutomaton &in, RegionAutomaton &out);
void getReachableStates (const NFA &A, std::vector<State> &out);

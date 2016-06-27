#pragma once

#include "types.hh"

// returns states in RA
// L is duplicated, because of pop_back
void calcL(const RegionAutomaton &RA,std::vector<std::vector<State> > &L, int &m);
// returns states in RA
void calcLs (const RegionAutomaton &RA,std::vector<std::vector<State> > &Ls,const State s,const int m,int &ms);

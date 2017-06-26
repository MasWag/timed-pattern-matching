#pragma once

#include <unordered_set>
#include "types.hh"
#include "zone.hh"
#include "automaton_operations.hh"

// returns states in RA
// L is duplicated, because of pop_back
/*!
  @brief calculate L' and m

  @todo Check if this algorithm is BFS?
  @todo any_of search can be memoized
  @todo test of endChars
 */
template<typename Abstraction>
void calcL(const AbstractionAutomaton<Abstraction> &RA, std::vector<std::pair<std::vector<State>, std::string> > &L, int &m, std::unordered_set<Alphabet> &endChars)
{
  // num of states
  const auto numOfRAStates = RA.abstractedStates.size();
  
  // calc revRA
  AbstractionAutomaton<Abstraction> revRA;
  reverseNFA (RA,revRA);
  revRA.abstractedStates = RA.abstractedStates;
  
  // calc acceptingStatesVector
  std::vector<bool> acceptingStatesVector;
  acceptingStatesVector.resize(numOfRAStates);
  for (std::size_t regionState = 0;regionState < numOfRAStates; regionState++) {
    acceptingStatesVector[regionState] = binary_search (RA.acceptingStates.begin(),
                                                      RA.acceptingStates.end(),regionState);
  }

  std::vector<State> reachableStates;
  getReachableStates (revRA,reachableStates);

  m = 0;
  std::vector<std::pair<std::vector<State>, std::string> > nextRuns;

  L.clear();
  for (const auto &initState: RA.initialStates) {
    if (any_of(reachableStates.begin(),reachableStates.end(),
               [&](RAState ras){
                 return revRA.abstractedStates[ras] == RA.abstractedStates[initState];})){
      L.push_back ({{initState}, ""});
    }
  }

  std::unordered_set<Alphabet> tmpEndChars;
  std::unordered_set<Alphabet> tmpEndChars0;
  // Breadth first search
  while (!L.empty()) {    
    endChars = std::move(tmpEndChars);
    tmpEndChars = std::move(tmpEndChars0);
    tmpEndChars0.clear ();
    nextRuns.clear ();
    for (const auto &sourceRun: L) {
      const RAState lastState = sourceRun.first.back();
      if(acceptingStatesVector[lastState]) goto end_of_loop;

      const auto &edges = RA.edges[lastState];
      for (const auto &edge: edges) {
        if (any_of(reachableStates.begin(),reachableStates.end(),
                   [&](RAState ras){
                     return revRA.abstractedStates[ras] == RA.abstractedStates[edge.target];})){
          std::pair<std::vector<State>,std::string> tmpRun = sourceRun;
          tmpEndChars0.insert (edge.c);
          tmpRun.first.push_back (edge.target);
          tmpRun.second.push_back (edge.c);
          nextRuns.push_back (tmpRun);
        }
      }
    }

    L = std::move(nextRuns);
    m++;
  }

  end_of_loop:

  // remove to make the size m
  for (auto& run: L) {
    run.first.pop_back ();
    run.second.pop_back ();
  }
  // remove duplicated items
  L.erase(unique (L.begin(), L.end()), L.end());
}


/*!
  @bref calculate Ls and min(m,m_s)

  @note ms is not m_s but min(m,m_s) in the paper.
  @todo ms seems different
  @note returns states in RA
*/
template<typename Abstraction>
void calcLs (const AbstractionAutomaton<Abstraction> &RA,std::vector<std::vector<State> > &Ls,const State s,const int m,int &ms)
{  
  // num of states
  const auto numOfRAStates = RA.abstractedStates.size();

  // calc revRA_s and acceptingStatesVector
  AbstractionAutomaton<Abstraction> revRA_s;
  reverseNFA (RA,revRA_s);
  revRA_s.abstractedStates = RA.abstractedStates;
  revRA_s.initialStates.clear();

  std::vector<bool> acceptingStatesVector;
  acceptingStatesVector.resize(numOfRAStates);
  for (std::size_t regionState = 0; regionState < numOfRAStates; regionState++) {
    if (RA.abstractedStates[regionState].first == s) {
      revRA_s.initialStates.push_back (regionState);
      acceptingStatesVector[regionState] = true;
    } else {
      acceptingStatesVector[regionState] = false;
    }
  }
  
  std::vector<State> reachableStates;
  getReachableStates (revRA_s,reachableStates);
  
  ms = 1;
  std::vector<std::vector<State> > nextRuns;
  
  Ls.clear();
  for (const auto &initState: RA.initialStates) {
    if (any_of(reachableStates.begin(),reachableStates.end(),
               [&](RAState ras){
                 return revRA_s.abstractedStates[ras] == RA.abstractedStates[initState];})){
    Ls.push_back ({initState});
    }
  }
  // calc ms and L's
  // BFS
  while (!Ls.empty()) {
    if (any_of (Ls.begin(), Ls.end (),
                [&] (std::vector<State> &s){return acceptingStatesVector[s.back()];})) {
      break;
    }
    if (m < ms + 1) {
      break;
    }

    for (const auto &sourceRun: Ls) {
      for (const auto &edge: RA.edges[sourceRun.back()]) {
        if (any_of(reachableStates.begin(),reachableStates.end(),
                   [&](RAState ras){
                     return revRA_s.abstractedStates[ras] == RA.abstractedStates[edge.target];})){
          std::vector<State> tmpRun = sourceRun;
          tmpRun.push_back (edge.target);
          nextRuns.push_back (tmpRun);
        }
      }
    }

    Ls = nextRuns;
    nextRuns.clear ();
    ms++;
  }
  std::sort (Ls.begin(), Ls.end ());
  Ls.erase(unique (Ls.begin(), Ls.end()), Ls.end());
}

template<typename Abstraction>
void calcL(const AbstractionAutomaton<Abstraction> &RA,std::vector<std::vector<State> > &L, int &m)
{
  // num of states
  const auto numOfRAStates = RA.abstractedStates.size();
  
  // calc revRA
  AbstractionAutomaton<Abstraction> revRA;
  reverseNFA (RA,revRA);
  revRA.abstractedStates = RA.abstractedStates;
  
  // calc acceptingStatesVector
  std::vector<bool> acceptingStatesVector;
  acceptingStatesVector.resize(numOfRAStates);
  for (std::size_t regionState = 0;regionState < numOfRAStates; regionState++) {
    acceptingStatesVector[regionState] = binary_search (RA.acceptingStates.begin(),
                                                      RA.acceptingStates.end(),regionState);
  }

  std::vector<State> reachableStates;
  getReachableStates (revRA,reachableStates);

  m = 0;
  std::vector<std::vector<State> > nextRuns;

  L.clear();
  for (const auto &initState: RA.initialStates) {
    if (any_of(reachableStates.begin(),reachableStates.end(),
               [&](RAState ras){
                 return revRA.abstractedStates[ras] == RA.abstractedStates[initState];})){
      L.push_back ({initState});
    }
  }

  // Breadth first search
  while (!L.empty()) {    
    nextRuns.clear ();
    for (const auto &sourceRun: L) {
      const RAState lastState = sourceRun.back();
      if(acceptingStatesVector[lastState]) goto end_of_loop;

      const auto &edges = RA.edges[lastState];
      for (const auto &edge: edges) {
        if (any_of(reachableStates.begin(),reachableStates.end(),
                   [&](RAState ras){
                     return revRA.abstractedStates[ras] == RA.abstractedStates[edge.target];})){
          std::vector<State> tmpRun = sourceRun;
          tmpRun.push_back (edge.target);
          nextRuns.push_back (tmpRun);
        }
      }
    }

    L = std::move(nextRuns);
    m++;
  }

  end_of_loop:
  // remove to make the size m
  for (auto& run: L) {
    run.pop_back ();
  }
}

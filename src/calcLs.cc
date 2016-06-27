#include <iostream>
#include "types.hh"
#include "automaton_operations.hh"

using namespace std;

/*!
  @brief calculate L' and m

  @todo Check if this algorithm is BFS?
  @todo any_of search can be memoized
 */
void calcL(const RegionAutomaton &RA,
            vector<vector<State> > &L,
            int &m)
{
  // num of states
  const auto numOfRAStates = RA.regionStates.size();
  
  // calc revRA
  RegionAutomaton revRA;
  reverseNFA (RA,revRA);
  revRA.regionStates = RA.regionStates;
  
  // calc acceptingStatesVector
  vector<bool> acceptingStatesVector;
  acceptingStatesVector.resize(numOfRAStates);
  for (std::size_t regionState = 0;regionState < numOfRAStates; regionState++) {
    acceptingStatesVector[regionState] = binary_search (RA.acceptingStates.begin(),
                                                      RA.acceptingStates.end(),regionState);
  }

  vector<State> reachableStates;
  getReachableStates (revRA,reachableStates);

  m = 0;
  vector<vector<State> > nextRuns;

  L.clear();
  for (const auto &initState: RA.initialStates) {
    if (any_of(reachableStates.begin(),reachableStates.end(),
               [&](RAState ras){
                 return revRA.regionStates[ras] == RA.regionStates[initState];})){
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
                     return revRA.regionStates[ras] == RA.regionStates[edge.target];})){
          vector<State> tmpRun = sourceRun;
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

/*!
  @bref calculate Ls and min(m,m_s)

  @note ms is not m_s but min(m,m_s) in the paper.
  @todo ms seems different
 */
void calcLs (const RegionAutomaton &RA,
             vector<vector<State> > &Ls,
             const TAState s,const int m,int &ms)
{  
  // num of states
  const auto numOfRAStates = RA.regionStates.size();

  // calc revRA_s and initialStatesVector
  RegionAutomaton revRA_s;
  reverseNFA (RA,revRA_s);
  revRA_s.regionStates = RA.regionStates;
  revRA_s.initialStates.clear();

  vector<bool> initialStatesVector;
  initialStatesVector.resize(numOfRAStates);
  for (std::size_t regionState = 0; regionState < numOfRAStates; regionState++) {
    if (RA.regionStates[regionState].first == s) {
      revRA_s.initialStates.push_back (regionState);
      initialStatesVector[regionState] = true;
    } else {
      initialStatesVector[regionState] = false;
    }
  }
  
  vector<State> reachableStates;
  getReachableStates (revRA_s,reachableStates);
  
  ms = 1;
  vector<vector<State> > nextRuns;
  
  Ls.clear();
  for (const auto &initState: RA.initialStates) {
    if (any_of(reachableStates.begin(),reachableStates.end(),
               [&](RAState ras){
                 return revRA_s.regionStates[ras] == RA.regionStates[initState];})){
    Ls.push_back ({initState});
    }
  }
  // calc ms and L's
  // BFS
  while (!Ls.empty()) {
    if (any_of (Ls.begin(), Ls.end (),
                [&] (vector<State> &s){return initialStatesVector[s.back()];})) {
      break;
    }
    if (m < ms + 1) {
      break;
    }

    for (const auto &sourceRun: Ls) {
      for (const auto &edge: RA.edges[sourceRun.back()]) {
        if (any_of(reachableStates.begin(),reachableStates.end(),
                   [&](RAState ras){
                     return revRA_s.regionStates[ras] == RA.regionStates[edge.target];})){
          vector<State> tmpRun = sourceRun;
          tmpRun.push_back (edge.target);
          nextRuns.push_back (tmpRun);
        }
      }
    }

    Ls = nextRuns;
    nextRuns.clear ();
    ms++;
  }
}

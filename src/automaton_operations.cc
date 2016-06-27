#include <utility>
#include <set>
#include <unordered_set>
#include "types.hh"

void reverseNFA (const NFA &in, NFA &out)
{
  out.initialStates = in.acceptingStates;
  out.acceptingStates = in.initialStates;
  out.edges.clear();
  out.edges.resize (in.edges.size());
  
  for (const auto &v : in.edges) {
    for (const auto &edge : v) {
      out.edges[edge.target].push_back 
        ({edge.target,edge.source,edge.c});
    }
  }
}

void getReachableStates (const NFA &A, std::vector<State> &reachableStates)
{
  if (A.initialStates.empty ()) {
    reachableStates.clear();
    return;
  }

  std::vector<State> states = A.initialStates;
  std::vector<State> next_states;
  bool finish = false;

  std::set<State> reachableStatesSet =
    {A.initialStates.begin (),A.initialStates.end ()};
  // calculate reachable states
  while (!finish) {
    finish = true;
    for (auto & state : states) {
      for (const auto &edge : A.edges[state]) {
        if (reachableStatesSet.find(edge.target) == reachableStatesSet.end()) {
          next_states.push_back (edge.target);
          finish = false;
          reachableStatesSet.insert(edge.target);
        }
      }
    }
    states = std::move(next_states);
    next_states.clear();
  }

  reachableStates = {reachableStatesSet.begin(), reachableStatesSet.end()};
}

void removeUnreachableStates (const RegionAutomaton &in, RegionAutomaton &out)
{
  std::vector<State> reachableStates;
  getReachableStates (in,reachableStates);


  // generate output
  out.initialStates.clear();
  out.initialStates.reserve (in.initialStates.size ());
  for (auto state: in.initialStates) {
    auto it = lower_bound(reachableStates.begin(), reachableStates.end(),state);
    if (*it == state)  {
      out.initialStates.push_back (static_cast<State>(it - reachableStates.begin ()));
    }
  }


  out.edges.clear();
  out.edges.resize(reachableStates.size());
  for (auto it = reachableStates.begin();
       it != reachableStates.end(); it++) {
    const State source = it-reachableStates.begin();
    for (const auto &edge : in.edges[*it]) {
      auto it0 = lower_bound(reachableStates.begin(), reachableStates.end(),edge.target);
      if (edge.target == *it0) {
        const State target = it0 - reachableStates.begin();
        out.edges[source].push_back ({source,target,edge.c});
      }
    }
  }

  out.acceptingStates.clear();
  out.acceptingStates.reserve (in.acceptingStates.size ());
  for (auto state: in.acceptingStates) {
    auto it = lower_bound(reachableStates.begin(), reachableStates.end(),state);
    if (*it == state)  {
      out.acceptingStates.push_back (static_cast<State>(it - reachableStates.begin ()));
    }
  }

  out.regionStates.resize(out.edges.size());
  for (auto it = reachableStates.begin (); it != reachableStates.end (); it++) {
    out.regionStates[it - reachableStates.begin ()] = in.regionStates[*it];
  }
}

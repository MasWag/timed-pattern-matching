#pragma once

#include "types.hh"

/*
  Specifications
  ==============
  * (s,s') is encoded as s + |S| * s'
  * x = x1 or x2 + |C|
  * in1 and in2 can be same.
*/
template <int NVar1,int NVar2>
void intersectionTA (const TimedAutomaton<NVar1> &in1, const TimedAutomaton<NVar2> &in2, TimedAutomaton<NVar1+NVar2> &out)
{
  //! in1.State -> in2.State -> out.State
  const auto toIState = [&] (State s1,State s2) {return s1 + in1.stateSize() * s2;};

  // make initial states
  out.initialStates.clear ();
  out.initialStates.reserve (in1.initialStates.size() * in2.initialStates.size());
  for (State s1: in1.initialStates) {
    for (State s2: in2.initialStates) {
      out.initialStates.push_back (toIState(s1,s2));
    }
  }

  // make accepting states
  out.acceptingStates.clear ();
  out.acceptingStates.reserve (in1.acceptingStates.size() * in2.acceptingStates.size());
  for (State s1: in1.acceptingStates) {
    for (State s2: in2.acceptingStates) {
      out.acceptingStates.push_back (toIState(s1,s2));
    }
  }

  // make max constraints
  out.max_constraints.reserve( in1.max_constraints.size() + in2.max_constraints.size() ); // preallocate memory
  out.max_constraints.insert( out.max_constraints.end(), in1.max_constraints.begin(), in1.max_constraints.end() );
  out.max_constraints.insert( out.max_constraints.end(), in2.max_constraints.begin(), in2.max_constraints.end() );

  // make edges
  out.edges.resize (in1.stateSize() * in2.stateSize());
  for (State s1 = 0; s1 < in1.stateSize(); s1++) {
    for (State s2 = 0; s2 < in2.stateSize(); s2++) {
      for (const auto &e1: in1.edges[s1]) {
        for (const auto &e2: in2.edges[s2]) {
          if (e1.c == e2.c) {
            typename TimedAutomaton<NVar1+NVar2>::Edge e;
            e.source = toIState (e1.source,e2.source);
            e.target = toIState (e1.target,e2.target);
            e.c = e1.c;

            //concat resetVars
            e.resetVars.reserve (e1.resetVars.size() + e2.resetVars.size());
            e.resetVars.insert (e.resetVars.end(),e1.resetVars.begin(), e1.resetVars.end());
            e.resetVars.insert (e.resetVars.end(),e2.resetVars.begin(), e2.resetVars.end());
            std::for_each (e.resetVars.begin () + e1.resetVars.size(),
                           e.resetVars.end(),
                           [] (char &v) { v += NVar1;});

            //concat constraints
            e.guard.reserve (e1.guard.size() + e2.guard.size());
            e.guard.insert (e.guard.end(),e1.guard.begin(), e1.guard.end());
            e.guard.insert (e.guard.end(),e2.guard.begin(), e2.guard.end());
            std::for_each (e.guard.begin () + e1.guard.size(),
                           e.guard.end(),
                           [] (Constraint &guard) { guard.x += NVar1;});

            out.edges[toIState(s1,s2)].push_back (e);
          }
        }
      }
    }
  }  
}

#pragma once

#include <iostream>
#include <chrono>
#include <cmath>
#include <climits>
#include <set>
#include "types.hh"

struct ansZone {
  std::pair<double,bool> upperBeginConstraint;
  std::pair<double,bool> lowerBeginConstraint;
  std::pair<double,bool> upperEndConstraint;
  std::pair<double,bool> lowerEndConstraint;
  std::pair<double,bool> upperDeltaConstraint;
  std::pair<double,bool> lowerDeltaConstraint;  
  inline bool operator == (const ansZone z) const {
    return upperBeginConstraint == z.upperBeginConstraint &&
      lowerBeginConstraint == z.lowerBeginConstraint &&
      upperEndConstraint == z.upperEndConstraint &&
      lowerEndConstraint == z.lowerEndConstraint &&
      upperDeltaConstraint == z.upperDeltaConstraint &&
      lowerDeltaConstraint == z.lowerDeltaConstraint;
  }
};

inline bool isValidConstraint (const std::pair<double,bool>& upperConstraint,
                             const std::pair<double,bool>& lowerConstraint)
{
  return upperConstraint.first > lowerConstraint.first ||
    (upperConstraint.first == lowerConstraint.first &&
     upperConstraint.second &&
     lowerConstraint.second);
}

inline void updateConstraint(std::pair<double,bool>& upperConstraint,
                      std::pair<double,bool>& lowerConstraint,
                      const Constraint &delta,
                      const double comparedValue)
{
  switch (delta.odr) {
  case Constraint::Order::gt:
    if (lowerConstraint.first < comparedValue) {
      lowerConstraint.first = comparedValue;
      lowerConstraint.second = 0;
    } else if (lowerConstraint.first == comparedValue) {
      lowerConstraint.second = 0;
    }
    break;
  case Constraint::Order::ge:
    if (lowerConstraint.first < comparedValue) {
      lowerConstraint.first = comparedValue;
      lowerConstraint.second = 1;
    }
    break;
  case Constraint::Order::lt:
    if (upperConstraint.first > comparedValue) {
      upperConstraint.first = comparedValue;
      upperConstraint.second = 0;
    } else if (upperConstraint.first == comparedValue) {
      upperConstraint.second = 0;
    }
    break;
  case Constraint::Order::le:
    if (upperConstraint.first > comparedValue) {
      upperConstraint.first = comparedValue;
      upperConstraint.second = 1;
    }
    break;
  }
}

template <int NVar>
void naive (std::vector<std::pair<Alphabet,double> > word,
              TimedAutomaton <NVar> A,
              std::vector<ansZone> &ans)
{
  struct InternalState{
    using Variables = char;
    State s;
    std::array<double,NVar> resetTime;
    std::pair<double,bool> upperConstraint;
    std::pair<double,bool> lowerConstraint;
  };

  auto start = std::chrono::system_clock::now();

  int i = word.size() - 1;
  std::vector<std::pair<std::pair<double,bool>,std::pair<double,bool> > > init;
  std::vector<InternalState> CStates;
  std::vector<InternalState> LastStates;
  const std::pair<double,bool> upperMaxConstraint = {INFINITY,false};
  const std::pair<double,bool> lowerMinConstraint = {0,true};
  for (const auto &s: A.initialStates) {
    for (const auto &e: A.edges[s]) {
      if (binary_search (A.acceptingStates.begin(), A.acceptingStates.end(),e.target) and e.c == '$') {
        // solve delta
        std::pair<double,bool> upperConstraint = upperMaxConstraint;
        std::pair<double,bool> lowerConstraint = lowerMinConstraint;
        for (const auto & constraint: e.guard) {
          updateConstraint (upperConstraint,lowerConstraint,constraint,constraint.c);

          if (isValidConstraint (upperConstraint,lowerConstraint)) {
            init.push_back ({upperConstraint,lowerConstraint});
          }
        }
      }
    }
  }
  
  ans.clear();
  ans.reserve(init.size());
  for (const auto &t: init) {
    ans.push_back ({
        upperMaxConstraint,
          {word[i].second,true},
          upperMaxConstraint,
            {word[i].second,false},
            t.first,t.second});
  }
  int j;
  while (i >= 0) {
    j = i;
    CStates.clear ();
    CStates.reserve(A.initialStates.size());
    if (i <= 0) {
      for (const auto& s: A.initialStates) {
        CStates.push_back ({s,{},{word[i].second,false},{0,true}});
      }
    } else {
      for (const auto& s: A.initialStates) {
        CStates.push_back ({s,{},{word[i].second,false},{word[i-1].second,true}});
      }
    }

    ans.reserve(ans.size() + init.size());
    if (i <= 0) {
      for (const auto& t: init) {
        ans.push_back ({{word[i].second,false},{0,true},{word[i].second,true},{0,false},t.first,t.second});
      }
    } else {
      for (const auto& t: init) {
        ans.push_back ({{word[i].second,false},{word[i-1].second,true},{word[i].second,true},{word[i-1].second,false},t.first,t.second});
      }
    }
    
    while (!CStates.empty () && j < word.size ()) {
      LastStates = CStates;
      CStates.clear ();
      const Alphabet c = word[j].first;
      const double t = word[j].second;
      
      for (const auto &state : LastStates) {
        const State s = state.s;
        
        for (const auto &edge : A.edges[s]) {
          if (edge.c == c) {

            std::pair<double,bool> upperBeginConstraint = state.upperConstraint;
            std::pair<double,bool> lowerBeginConstraint = state.lowerConstraint;
            bool transitable = true;
            
            for (const auto& delta: edge.guard) {
              if (state.resetTime[delta.x]) {
                if (!delta.satisfy(t - state.resetTime[delta.x])) {
                  transitable = false;
                  break;
                }
              } else {
                const double delta_t = t - delta.c;
                switch (delta.odr) {
                case Constraint::Order::lt:
                case Constraint::Order::le:
                  if (lowerBeginConstraint.first < delta_t) {
                    lowerBeginConstraint.first = delta_t;
                    lowerBeginConstraint.second = 
                      delta.odr == Constraint::Order::le;
                  } else if (lowerBeginConstraint.first == delta_t) {
                    lowerBeginConstraint.second = 
                      lowerBeginConstraint.second && delta.odr == Constraint::Order::le;
                  }
                  break;
                case Constraint::Order::gt:
                case Constraint::Order::ge:
                  if (upperBeginConstraint.first > delta_t) {
                    upperBeginConstraint.first = delta_t;
                    upperBeginConstraint.second = 
                      delta.odr == Constraint::Order::ge;
                  } else if (upperBeginConstraint.first == delta_t) {
                    upperBeginConstraint.second = 
                      upperBeginConstraint.second && delta.odr == Constraint::Order::ge;
                  }
                  break;
                }
              }
            }

            if (!transitable || !isValidConstraint (upperBeginConstraint,lowerBeginConstraint)) {
              continue;
            }
            
            auto tmpResetTime = state.resetTime;
            for (auto i : edge.resetVars) {
              tmpResetTime[i] = t;
            }
            
            CStates.push_back ({edge.target,tmpResetTime,upperBeginConstraint,lowerBeginConstraint});
            
            for (const auto &edge_f: A.edges[edge.target]) {
              if (std::binary_search(A.acceptingStates.begin(),
                                     A.acceptingStates.end(),edge_f.target) and edge_f.c == '$') {
                std::pair<double,bool> upperEndConstraint = upperMaxConstraint;
                std::pair<double,bool> lowerEndConstraint = {word[j].second,false};
                std::pair<double,bool> upperDeltaConstraint = upperMaxConstraint;
                std::pair<double,bool> lowerDeltaConstraint = lowerMinConstraint;
                
                if (j != word.size() - 1) {
                  upperEndConstraint = {word[j+1].second,true};
                }

                // solve delta
                for (const auto& delta: edge_f.guard) {
                  if (tmpResetTime[delta.x]) {
                    const double delta_t = delta.c + tmpResetTime[delta.x];
                    updateConstraint (upperEndConstraint,lowerEndConstraint,delta,delta_t);
                  } else {
                    updateConstraint (upperDeltaConstraint,lowerDeltaConstraint,delta,delta.c);
                  }
                }

                if (!isValidConstraint (upperBeginConstraint,lowerBeginConstraint) ||
                    !isValidConstraint (upperEndConstraint,lowerEndConstraint) ||
                    !isValidConstraint (upperDeltaConstraint,lowerDeltaConstraint)) {
                  continue;
                }

                ans.push_back ({upperBeginConstraint,lowerBeginConstraint,
                      upperEndConstraint,lowerEndConstraint,
                      upperDeltaConstraint,lowerDeltaConstraint});
              }
            }
          }
        }
      }
      j++;
    }
    i -= 1;
  }
  auto end = std::chrono::system_clock::now();
  auto dur = end - start;
  auto nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count();
  std::cout << "main computation: " << nsec / 1000000.0 << " ms" << std::endl;
}

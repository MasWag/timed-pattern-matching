#pragma once

#include <tuple>
#include <boost/unordered_map.hpp>
#include "types.hh"

/*!
  @brief given two runs r and r' of Region automata A and B, check if (r,r') is a run of A x B

  @note This class is a function object.
 */
class PartialRunChecker
{
public:
  
  PartialRunChecker (RegionAutomaton &RA2,RegionAutomaton &RA,const TAState TAStateSize) : NVar (RA.regionStates.front().second.max_constraints.size()),
                                                                                           RA2(RA2),
                                                                                           RA(RA), 
                                                                                           TAStateSize (TAStateSize)
  {
    if (NVar == 0) {
      for (auto it = RA2.regionStates.begin ();it != RA2.regionStates.end (); it++) {      
        m_initialStates[std::make_tuple(it->first,it->second.toTuple(),it->second.toTuple())].push_back(it-RA2.regionStates.begin ());
      }
    } else {
      for (auto it = RA2.regionStates.begin ();it != RA2.regionStates.end (); it++) {
        std::shared_ptr<Region> re1,re2;
        it->second.cutVars (re1,0,NVar-1);
        it->second.cutVars (re2,NVar,NVar*2-1);
      
        m_initialStates[std::make_tuple(it->first,re1->toTuple(),re2->toTuple())].push_back(it-RA2.regionStates.begin ());
      }
    }
  }  
  bool operator()(const std::vector<RAState> &r1,const std::vector<RAState> &r2) {
    if (r1.size () != r2.size ()) {
      return false;
    }

    TAState iState = toIState (r1[0],r2[0]);

    Region region1 = RA.regionStates[r1[0]].second;
    Region region2 = RA.regionStates[r2[0]].second;

    std::vector<RAState> CStates = m_initialStates[std::make_tuple(iState,region1.toTuple(),region2.toTuple())];

    if (NVar == 0) {
      for (std::size_t i = 1; i < r1.size(); i++) {
        const TAState iState = toIState (r1[i],r2[i]);

        std::vector<RAState> NStates;
      
        for (const RAState s: CStates ) {
          for (const NFA::Edge &e: RA2.edges[s]) {
            if (RA2.regionStates[e.target].first == iState) {
              NStates.push_back (e.target);
            }
          }
        }
        if (NStates.empty()) {
          return false;
        }
        CStates = NStates;
      }
    } else {
      for (std::size_t i = 1; i < r1.size(); i++) {
        const TAState iState = toIState (r1[i],r2[i]);

        const Region region1 = RA.regionStates[r1[i]].second;
        const Region region2 = RA.regionStates[r2[i]].second;
        std::vector<RAState> NStates;
      
        for (const auto s: CStates ) {
          for (const NFA::Edge &e: RA2.edges[s]) {
            std::shared_ptr<Region> re1,re2;
            RA2.regionStates[e.target].second.cutVars (re1,0,NVar-1);
            RA2.regionStates[e.target].second.cutVars (re2,NVar,NVar*2-1);
        
            if (RA2.regionStates[e.target].first == iState && *re1 == region1 && *re2 == region2) {
              NStates.push_back (e.target);
            }
          }
        }
        if (NStates.empty()) {
          return false;
        }
        CStates = NStates;
      }
    }

    return true;    
  }
  
private:
  const std::size_t NVar;
  const TAState TAStateSize;
  RegionAutomaton &RA2;
  RegionAutomaton &RA;
  // (TAState,([(Int,Int)],[[Int]], [Int],),([(Int,Int)],[[Int]], [Int])) -> [RAState]
  // This seems ((s1,s2),\alpha1,\alpha2) -> ((s1,\alpha1),(s2,\alpha2))
  boost::unordered_map<std::tuple<TAState,std::tuple<std::vector< std::pair<int,int> >, std::list<std::list<int> >, std::vector<int> >,std::tuple<std::vector< std::pair<int,int> >, std::list<std::list<int> >, std::vector<int> > >,std::vector<RAState> > m_initialStates;
  inline TAState toIState(RAState s1,RAState s2) const {
    TAState taState1 = RA.regionStates[s1].first;
    TAState taState2 = RA.regionStates[s2].first;
    
    return taState1 + TAStateSize * taState2;
  }
};

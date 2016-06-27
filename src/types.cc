#include <iostream>
#include "types.hh"

inline bool Region::self_successor () const 
{
  for (const auto &p: integer_parts) {
    if (p.first == p.second) {
      return false;
    }      
  }
  return true;
}

/*!
  @brief calculate the next larger region
  @param [out] next 
  @param [out] self_successor True when this region is also a successor of itself.
  @retval true there exist larger regions
  @retval false there is no larger region
 */
bool Region::nextRegion (Region &next,bool &self_successor) const 
{
  self_successor = true;
  std::list<int> next_fracs;

  next.integer_parts = integer_parts;
  next.frac_order = frac_order;
  next.max_constraints = max_constraints;

  // add a variable x such that x = t.
  for (int i = 0; i < integer_parts.size();i++) {
    if (integer_parts[i].first == integer_parts[i].second) {
      next.integer_parts[i].second += 1;
      self_successor = false;
      if (integer_parts[i].first < max_constraints[i]) {
        next_fracs.push_back (i);
      }
    }
  }

  if (frac_order.empty() && self_successor) {
    return false;
  }

  if (self_successor) {
    // When self_successor --- that means there is no variable x such that x = t --- pop the largest fracs from frac_order.
    for (auto & index: frac_order.back()) {
      next.integer_parts[index].first += 1;
    }
    next.frac_order.pop_back();
  } else {
    // Change x = t to t < x < t + 1
    if (!next_fracs.empty()) {
      next.frac_order.push_front (std::move(next_fracs));
    }
  }
  return true;
}

// Maybe not work
bool RegionAutomaton::isPartialRun (std::vector<State> run) const 
{
  std::vector<State> CStates;
  std::vector<State> NStates;
  for (auto it = regionStates.begin ();it != regionStates.end (); it++) {
    if (it->first == run.front()) {
      CStates.push_back (it-regionStates.begin ());
    }
  }

  for (auto it = run.begin (); it != run.end () - 1; it++) {
    for (const auto s: CStates ) {
      for (const Edge &e: edges[s]) {
        if (regionStates[e.target].first == *(it+1)) {
          NStates.push_back (e.target);
        }
      }
    }
    if (NStates.empty()) {
      return false;
    }
    CStates = NStates;
    NStates.clear();
  }

  return true;
}

std::ostream& operator << (std::ostream& os, const Region& r)
{
  for (int i = 0; i < r.integer_parts.size();++i) {
    os << r.integer_parts[i].first << "<" 
       << i << "<"
       << r.integer_parts[i].second << std::endl;
  }
  for (auto f : r.frac_order ) {
    for (auto i : f) {
      os << "frac(" << i << "),";
    }
    os << "<" << std::endl;
  }
  return os;
};

bool RegionAutomaton::isPartialRun2 (const RegionAutomaton &RA,
                                     const std::vector<RAState> &r1,
                                     const std::vector<RAState> &r2,
                                     const TAState TAStateSize) //const 
{
  if (r1.size () != r2.size ()) {
    return false;
  }
  std::vector<RAState> CStates;

  TAState taState1 = RA.regionStates[r1.front()].first;
  Region region1 = RA.regionStates[r1.front()].second;
  TAState taState2 = RA.regionStates[r2.front()].first;
  Region region2 = RA.regionStates[r2.front()].second;
  
  const auto NVar = region1.max_constraints.size();
  const auto toIState = [&](RAState s1,RAState s2) -> RAState
    {return s1 + TAStateSize * s2;};

  TAState iState = toIState (taState1,taState2);
  for (auto it = regionStates.begin ();it != regionStates.end (); it++) {
    std::shared_ptr<Region> re1,re2;
    it->second.cutVars (re1,0,NVar-1);
    it->second.cutVars (re2,NVar,NVar*2-1);
    
    if (it->first == iState && *re1 == region1 && *re2 == region2) {
      CStates.push_back (it-regionStates.begin ());
    }
  }

  for (int i = 1; i < r1.size(); i++) {
    const auto taState1 = RA.regionStates[r1[i]].first;
    const auto taState2 = RA.regionStates[r2[i]].first;
    const auto iState = toIState (taState1,taState2);

    const auto region1 = RA.regionStates[r1[i]].second;
    const auto region2 = RA.regionStates[r2[i]].second;
    std::vector<RAState> NStates;
    for (const auto s: CStates ) {
      for (const Edge &e: edges[s]) {
        std::shared_ptr<Region> re1,re2;
        if (NVar == 0) {
          re1 = std::make_shared<Region>(regionStates[e.target].second);
          re2 = std::make_shared<Region>(regionStates[e.target].second);
        } else {
          regionStates[e.target].second.cutVars (re1,0,NVar-1);
          regionStates[e.target].second.cutVars (re2,NVar,NVar*2-1);
        }
        
        if (regionStates[e.target].first == iState && *re1 == region1 && *re2 == region2) {
          NStates.push_back (e.target);
        }
      }
    }
    if (NStates.empty()) {
      return false;
    }
    CStates = NStates;
  }
  return true;
}

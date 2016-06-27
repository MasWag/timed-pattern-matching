#pragma once

#include <unordered_set>
#include <boost/unordered_map.hpp>
#include <utility>
#include <tuple>
#include "types.hh"
#include <boost/functional/hash.hpp>

/*!
  @brief Generate a region automaton from a timed automaton
  @tparam NVar the number of variable in TA

  TA to RA adds states with BFS. Initial configuration is the initial states of RA. The RA contain only the states reachable from initial states.
  THIS DOES NOT MAKE INITIAL/ACCEPTING STATES!!
 */
template <int NVar>
void ta_to_ra_naive (const TimedAutomaton<NVar> &TA,RegionAutomaton &RA)
{
  using Variables = char;
  State numOfStates;

  // make all integer parts
  std::vector<std::vector<std::pair<int,int> > > integer_parts;  
  integer_parts.resize(TA.max_constraints.size());
  for (int i = 0; i < integer_parts.size(); i++) {
    integer_parts[i].reserve((TA.max_constraints[i] + 1) * 2);
    std::pair<int,int> p = {0,0};
    while (p.first <= TA.max_constraints[i]) {
      integer_parts[i].push_back (p);
      if (p.first == p.second) {
        p.second++;
      } else {
        p.first++;
      }
    }
  }

  std::vector<Region> regions;
  std::vector<std::size_t> indices;
  indices.resize(integer_parts.size());
  while (1) {
    Region r;
    r.max_constraints = TA.max_constraints;
    r.frac_order.clear();
    std::vector<int> fracs;
    fracs.reserve (integer_parts.size());

    r.integer_parts.resize(integer_parts.size());
    for (std::size_t i = 0;i < integer_parts.size();i++) {
      r.integer_parts[i] = integer_parts[i][indices[i]];
      if (r.integer_parts[i].first != r.integer_parts[i].second && r.integer_parts[i].second <= r.max_constraints[i]) {
        fracs.push_back (i);
      }
    }
    
    // calc fracs
    if (fracs.empty()) {
      regions.push_back (r);
    } else {
      std::vector<std::vector<std::vector<int> > > c_orders = {{fracs}};
      std::unordered_set<std::vector<std::vector<int> >, boost::hash<std::vector<std::vector<int> > > > addedOrders;
      addedOrders.insert ({fracs});
      auto tmp_region = r;
      tmp_region.frac_order.push_back({fracs.begin(),fracs.end()});
      regions.push_back (std::move(tmp_region));
      while (!c_orders.empty()) {
        std::vector<std::vector<std::vector<int> > > next_orders;
        for (const auto &order: c_orders) {
          for (auto i = 0; i < order.size();i++) {
            for (auto j = 0; j < order[i].size();j++) {
              auto tmp_region = r;
              auto tmp_order = order;
              auto tmp = tmp_order[i][j];
              tmp_order[i].erase(tmp_order[i].begin()+j);
              if (i + 1 < order.size()) {
                tmp_order[i+1].push_back (tmp);
                sort(tmp_order[i+1].begin(), tmp_order[i+1].end());
              } else {
                tmp_order.push_back({tmp});
              }
              if (tmp_order[i].empty()) {
                tmp_order.erase(tmp_order.begin() + i);
              }
              // if tmp_order is new
              if (addedOrders.find (tmp_order) == addedOrders.end()) {
                addedOrders.insert (tmp_order);
                for (const auto& odr: tmp_order) {
                  tmp_region.frac_order.push_back({odr.begin(),odr.end()});
                }
                next_orders.push_back (std::move(tmp_order));
                // std::list<std::list<int> > l = {tmp_order.begin(),tmp_order.end()};
                // r.frac_order = std::move(l);
                regions.push_back (std::move(tmp_region));
              }
            }
          }
        }
        c_orders = std::move(next_orders);
      }
    }

    // increment indices
    std::size_t i;
    for (i = 0;i < indices.size();i++) {
      if(indices[i]+1 < integer_parts[i].size()) {
        indices[i]++;
        break;
      } else {
        indices[i] = 0;
      }
    }
    if (i == indices.size()) {
      break;
    }
  }

  // std::cout << "# of regions: " << regions.size() << std::endl;
  // added regions  
  RA.regionStates.reserve (TA.edges.size() * regions.size());

  boost::unordered_map<std::tuple<TAState,std::vector< std::pair<int,int> >, std::list<std::list<int> > >, RAState> revRegionStates;
  for(TAState s = 0; s < TA.edges.size();s++) {
    for (const auto &region: regions) {
      revRegionStates[std::make_tuple(s,region.integer_parts,region.frac_order)] = RA.regionStates.size();
      RA.regionStates.push_back ({s,region});
    }
  }

  // make edges
  RA.edges.resize (TA.edges.size() * regions.size());
  
  for (RAState rs = 0; rs < RA.regionStates.size();rs++) {
    const auto &region = RA.regionStates[rs];
    Region now,next;
    bool ss;
    now = region.second;
    bool f = now.nextRegion (next,ss);
    
    next = ss ? now : next;
    if (!f && !ss)
      continue;
    do {
      now = std::move(next);
      bool too_large = true;
      const Region::Interpretation &val = now;
      for (const auto &e : TA.edges.at(region.first)) {
#ifdef DEBUG
        cout << (int)get<1>(s) << ":" 
             << (int)e.source << "->"
             << (int)e.target << " "
             << (e.guard(val) ? "true" : "false")
             << endl;
#endif
        if (std::all_of(e.guard.begin(),e.guard.end(),
                        [val] (const Constraint &delta){return delta(val) == Order::EQ;})) {
          too_large = false;
          Region nregion = now;
          for (auto i : e.resetVars) {
            nregion.integer_parts[i] = {0,0};
            for (auto it0 = nregion.frac_order.begin();
                 it0 != nregion.frac_order.end(); it0++) {
              auto it = it0->begin();
              // This seems unsorted but I don't know why.
              if ((it = find (it0->begin(), it0->end(),i)) != it0->end()) {
                //                if (!(binary_search (it0->begin(), it0->end(),i))) {
                it0->erase(it);
                if (it0->empty() ) {
                  nregion.frac_order.erase(it0);
                }
                break;
              }
            }
          }
        
          RAState raTarget = revRegionStates[std::make_tuple(e.target, nregion.integer_parts,nregion.frac_order)];
          //          RAState raTarget = find (RA.regionStates.begin(),RA.regionStates.end(),std::make_pair(e.target,nregion)) - RA.regionStates.begin();
          // TODO: edges are duplicated.
          RA.edges[rs].push_back ({rs,raTarget,e.c});
        } else if (std::none_of(e.guard.begin(), e.guard.end(),
                                [val](const Constraint &delta){return delta (val) == Order::GT;})) {
          too_large = false;
        }
      }
      if (too_large) break;
    } while(now.nextRegion (next,ss));
  }
}


#pragma once

#include <cstdlib>
#include <utility>
#include <tuple>
#include <numeric>

#include <boost/unordered_map.hpp>
#include "types.hh"

/*!
  @brief Generate a region automaton from a timed automaton
  @tparam NVar the number of variable in TA

  TA to RA adds states with BFS. Initial configuration is the initial states of RA. The RA contain only the states reachable from initial states.
 */
template <int NVar>
void ta_to_ra (const TimedAutomaton<NVar> &TA,RegionAutomaton &RA)
{
  //! number of states of RA
  State numOfStates;
  Region initialRegion;
  initialRegion.integer_parts.resize (NVar);
  fill(initialRegion.integer_parts.begin(), 
       initialRegion.integer_parts.end(),
       std::make_pair(0,0));
  initialRegion.max_constraints = TA.max_constraints;

  // make initial state
  numOfStates = TA.initialStates.size();
  RA.edges.resize(numOfStates);
  RA.initialStates.resize (numOfStates);
  iota (RA.initialStates.begin(), RA.initialStates.end(),0);

  RA.abstractedStates.reserve (numOfStates);
  for (const auto &conf : TA.initialStates ) {
    RA.abstractedStates.push_back (std::make_pair(conf,initialRegion));
  }

  /*!
    @brief Current configuration of BFS
    
    A configuration consistes of a tuple ((conf,\alpha),conf,alpha). Remark (conf,\alpha) is not a pair but just a number representing a state of RA.
   */
  std::vector<std::tuple<RAState,TAState,Region> > nextConf;
  nextConf.resize (RA.abstractedStates.size());
  for (std::size_t i = 0; i < nextConf.size();i++) {
    nextConf[i] = std::make_tuple(i,RA.abstractedStates[i].first,RA.abstractedStates[i].second);
  }
  
  /*! 
    @brief translater from TAState and Region to its corresponding state in RA.

    The type is like this.
    (TAState,([Int,Int],[[Int]],[Int])) -> RAState
  */
  boost::unordered_map<std::pair<TAState,std::tuple<std::vector< std::pair<int,int> >, std::list<std::list<int> >, std::vector<int> > >,RAState> regions_in_ra;
  while (!nextConf.empty ()) {
    std::vector<std::tuple<State,State,Region> > currentConf = nextConf;
    nextConf.clear();
    for (const auto &conf : currentConf) {
      Region nowRegion,nextRegion;
      bool self_successor;
      nowRegion = std::get<2>(conf);
      const bool existsNextRegion = nowRegion.nextRegion (nextRegion,self_successor);

      nextRegion = self_successor ? nowRegion : nextRegion;
      if (!existsNextRegion && !self_successor)
        break;
      do {
        nowRegion = std::move(nextRegion);
        bool tooLarge = true;
#ifdef DEBUG
        cout << "====================="<< endl;
#endif
        const Region::Interpretation val = nowRegion;
        for (const auto &edge : TA.edges.at(std::get<1>(conf))) {
#ifdef DEBUG
          cout << (int)get<1>(conf) << ":" 
               << (int)edge.source << "->"
               << (int)edge.target << " "
               << (edge.guard(val) ? "true" : "false")
               << endl;
#endif
          if (std::all_of(edge.guard.begin(),edge.guard.end(),
                          [&val] (const Constraint &delta){return delta(val) == Order::EQ;})) {
            tooLarge = false;
            Region targetRegion = nowRegion;
            for (auto x : edge.resetVars) {
              targetRegion.integer_parts[x] = {0,0};
              for (auto it = targetRegion.frac_order.begin();
                   it != targetRegion.frac_order.end(); it++) {
                const auto initialNum = it->size();
                it->remove(x);
                if (initialNum != it->size()) {
                  if (it->empty() ) {
                    targetRegion.frac_order.erase(it);
                  }
                  break;
                }
              }
            }

#ifdef DEBUG
            cout << "nowRegion" << endl;
            cout << nowRegion << endl;

            cout << "targetRegion" << endl;
            cout << targetRegion << endl;
#endif            
            // nextRegion state is new
            const auto targetRegionState = std::make_pair(edge.target,std::make_tuple(targetRegion.integer_parts,targetRegion.frac_order,targetRegion.max_constraints));
            const auto targetStateInRA = regions_in_ra.find(targetRegionState);
#ifdef DEBUG
            cout << (targetStateInRA == RA.abstractedStates.end()) << endl;
            cout << (int)get<0>(conf) << "-" << edge.c << ">" << static_cast<int>(targetStateInRA - RA.abstractedStates.begin()) << endl;
#endif
            // targetRegionState is already added
            if (targetStateInRA != regions_in_ra.end()) {
              RA.edges[std::get<0>(conf)].push_back ({std::get<0>(conf),targetStateInRA->second,edge.c});
            } else {
#ifdef DEBUG
              cout << "added " << (int)numOfStates << " = " << targetStateInRA - RA.abstractedStates.begin()<< endl;
#endif
              RA.edges[std::get<0>(conf)].push_back ({std::get<0>(conf),static_cast<State>(numOfStates),edge.c});
              if (binary_search (TA.acceptingStates.begin(), 
                                 TA.acceptingStates.end (),edge.target)) {
                RA.acceptingStates.push_back (numOfStates);
              }
              regions_in_ra[targetRegionState] = numOfStates;
              RA.abstractedStates.push_back (std::make_pair (edge.target,targetRegion));
              nextConf.push_back (std::make_tuple (numOfStates,edge.target,targetRegion));
              numOfStates++;
              RA.edges.resize(numOfStates);
            }
          } else if (std::none_of(edge.guard.begin(), edge.guard.end(),
                                  [val](const Constraint &delta){return delta (val) == Order::GT;})) {
            tooLarge = false;
          }
        }
        if (tooLarge) break;
      } while(nowRegion.nextRegion (nextRegion,self_successor));
    }
    //    std::cout << numOfStates << std::endl;
  }

}

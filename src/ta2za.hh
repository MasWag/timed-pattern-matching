#pragma once

#include <cstdlib>
#include <utility>
#include <tuple>
#include <numeric>

#include <boost/unordered_map.hpp>
#include "types.hh"
#include "zone.hh"

/*!
  @brief Generate a zone automaton from a timed automaton
  @tparam NVar the number of variable in TA

  TA to ZA adds states with BFS. Initial configuration is the initial states of ZA. The ZA contain only the states reachable from initial states.
 */
template <int NVar>
void ta2za (const TimedAutomaton<NVar> &TA,ZoneAutomaton &ZA, Zone initialZone = Zone::zero(NVar + 1))
{
  auto initialStates = TA.initialStates;
  if (!ZA.abstractedStates.empty()) {
    for (auto it = initialStates.begin(); it != initialStates.end();) {
      if (std::find(ZA.abstractedStates.begin(), ZA.abstractedStates.end(),std::pair<ZAState,Zone>{TAState(*it),initialZone}) != ZA.abstractedStates.end()) {
        it = initialStates.erase(it);
      } else {
        it++;
      }
    }
  }
  if (initialStates.empty()) {
    return;    
  }

  ZA.numOfVariables = NVar;
  //! number of states of ZA
  const State numOfStatesInitial = ZA.abstractedStates.size();
  State numOfStates = numOfStatesInitial;
  if (NVar > 0) {
    initialZone.M = Bounds{*std::max_element(TA.max_constraints.begin(), TA.max_constraints.end()), true};
  } else {
    initialZone.M = Bounds(0, true);
  }

  // make initial state
  numOfStates += initialStates.size();
  ZA.edges.resize(numOfStates);
  const State numOfInitialStatesInitial = ZA.initialStates.size();
  ZA.initialStates.resize (numOfStates);
  iota (ZA.initialStates.begin() + numOfInitialStatesInitial, ZA.initialStates.end(),numOfStatesInitial);

  ZA.abstractedStates.reserve (numOfStates);
  for (const auto &conf : initialStates ) {
    ZA.abstractedStates.push_back (std::make_pair(conf,initialZone));
  }

  /*!
    @brief Current configuration of BFS
    
    A configuration consistes of a tuple ((conf,\alpha),conf,alpha). Remark (conf,\alpha) is not a pair but just a number representing a state of RA.
   */
  std::vector<std::tuple<RAState,TAState,Zone> > nextConf;
  nextConf.resize (initialStates.size());
  for (std::size_t i = 0; i < nextConf.size();i++) {
    nextConf[i] = std::make_tuple(i + numOfStatesInitial, 
                                  ZA.abstractedStates[i+numOfStatesInitial].first,
                                  ZA.abstractedStates[i+numOfStatesInitial].second);
  }
  
  /*! 
    @brief translater from TAState and Region to its corresponding state in RA.

    The type is like this.
    (TAState,Zone) -> RAState
  */
  while (!nextConf.empty ()) {
    std::vector<std::tuple<RAState,TAState, Zone> > currentConf = nextConf;
    nextConf.clear();
    for (const auto &conf : currentConf) {
      Zone nowZone = std::get<2>(conf);
      nowZone.elapse();
      for (const auto &edge : TA.edges.at(std::get<1>(conf))) {
        Zone nextZone = nowZone;
        
        for (const auto &delta : edge.guard) {
          switch (delta.odr) {
          case Constraint::Order::lt:
            nextZone.tighten(delta.x,-1,{delta.c, false});
            break;
          case Constraint::Order::le:
            nextZone.tighten(delta.x,-1,{delta.c, true});
            break;
          case Constraint::Order::gt:
            nextZone.tighten(-1,delta.x,{-delta.c, false});
            break;
          case Constraint::Order::ge:
            nextZone.tighten(-1,delta.x,{-delta.c, true});
            break;
          }
        }

        if (nextZone.isSatisfiable()) {
          for (auto x : edge.resetVars) {
            nextZone.reset(x);
          }
          nextZone.abstractize();
          nextZone.canonize();
          // nextRegion state is new
          const auto targetStateInZA = ZA.zones_in_za.find(std::make_pair(edge.target, nextZone.toTuple()));

          // targetRegionState is already added
          if (targetStateInZA != ZA.zones_in_za.end()) {
            const NFA::Edge newEdge = {std::get<0>(conf),targetStateInZA->second,edge.c};
            ZA.edges[std::get<0>(conf)].push_back (newEdge);
            ZoneAutomaton::TAEdge taEdge = {edge.source, edge.target, edge.c, edge.resetVars, edge.guard};
            ZA.edgeMap[newEdge.toTuple()] = taEdge;
            
          } else {
            const NFA::Edge newEdge = {std::get<0>(conf),static_cast<State>(numOfStates),edge.c};
            ZA.edges[std::get<0>(conf)].push_back (newEdge);
            ZoneAutomaton::TAEdge taEdge = {edge.source, edge.target, edge.c, edge.resetVars, edge.guard};
            ZA.edgeMap[newEdge.toTuple()] = taEdge;
            
            if (binary_search (TA.acceptingStates.begin(), 
                               TA.acceptingStates.end (),edge.target)) {
              ZA.acceptingStates.push_back (numOfStates);
            }
            ZA.zones_in_za[std::make_pair(edge.target, nextZone.toTuple())] = numOfStates;
            ZA.abstractedStates.push_back (std::make_pair (edge.target,nextZone));
            nextConf.push_back (std::make_tuple (numOfStates,edge.target, nextZone));
            numOfStates++;
            ZA.edges.resize(numOfStates);
          }
        }
      }
    }
  }
}

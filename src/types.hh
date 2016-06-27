#pragma once

#include <iostream>
#include <memory>
#include <valarray>
#include <functional>
#include <utility>
#include <list>
#include <cstdint>
#include <vector>

#include <boost/optional.hpp>

typedef char Alphabet;
typedef uint32_t State;
typedef State TAState;
typedef State RAState;

//! @brief The return values of comparison of two values. Similar to strcmp.
enum class Order {
  LT,EQ,GT
};

inline bool toBool (Order odr) {
  return odr == Order::EQ;
}

struct Constraint {
  using Variables = char;
  enum class Order {
    lt,le,ge,gt
  };
    
  Variables x;
  Order odr;
  int c;

  bool satisfy (double d) const {
    switch (odr) {
    case Order::lt:
      return d < c;
    case Order::le:
      return d <= c;
    case Order::gt:
      return d > c;
    case Order::ge:
      return d >= c;
    }
  }
  using Interpretation = std::vector<double>;
  ::Order operator() (Interpretation val) const {
    if (satisfy (val.at (x))) {
      return ::Order::EQ;
    } else if (odr == Order::lt || odr == Order::le) {
      return ::Order::GT;
    } else {
      return ::Order::LT;
    }
  }
};

class ConstraintMaker {
  using Variables = char;
  Variables x;
public:
  ConstraintMaker(Variables x) : x(x) {}
  Constraint operator<(int c) {
    return Constraint {x,Constraint::Order::lt,c};
  }
  Constraint operator<=(int c) {
    return Constraint {x,Constraint::Order::le,c};
  }
  Constraint operator>(int c) {
    return Constraint {x,Constraint::Order::gt,c};
  }
  Constraint operator>=(int c) {
    return Constraint {x,Constraint::Order::ge,c};
  }
};

template <int NumOfVariables>
struct TimedAutomaton {
  using Variables = char;
  // arrayとどっちがいいかしらない
  using Interpretation = std::array<double,NumOfVariables>;

  struct Edge {
    State source;
    State target;
    Alphabet c;
    std::vector<Alphabet> resetVars;
    std::vector<Constraint> guard;
  };

  // This must be sorted.
  std::vector<State> initialStates;
  // Keys are source nodes.
  std::vector<std::vector<Edge> > edges;
  // This must be sorted.
  std::vector<State> acceptingStates;

  std::vector<int> max_constraints;
  inline size_t stateSize() const {return edges.size ();}
};

struct Region {
  using Interpretation = std::vector<double>;
  // (2,3) -> (2,3), (2,2) -> [2,2], (c,c>) -> (c,inf)  
  std::vector< std::pair<int,int> > integer_parts;
  // front() is the least.
  // each vector is sorted
  std::list<std::list<int> > frac_order;

  std::vector<int> max_constraints;

  /*!
    @brief generate next (least time-successor) region

    @note  next region cannot be now itself.

    @retval true  next region does exist
    @retval false next region does not exist
  */
  bool nextRegion (Region &next,bool & ss) const;

  inline bool self_successor () const;

  inline bool operator == (const Region &r) const {
    return integer_parts == std::move(r.integer_parts) && frac_order == std::move(r.frac_order);
  }
  inline bool operator == (const Region &&r) const {
    return integer_parts == std::move(r.integer_parts) && frac_order == std::move(r.frac_order);
  }

  operator Interpretation() const {
    Interpretation ret;
    ret.resize (integer_parts.size());
    for (std::size_t i = 0;i < ret.size();++i) {
      ret[i] = (integer_parts[i].first + integer_parts[i].second) * 0.5;
    }
    return ret;
  }

  inline bool operator> (const Region &r) const {
    return integer_parts > r.integer_parts;
  }
  inline bool operator> (const Region &&r) const {
    return integer_parts > r.integer_parts;
  }

  inline void cutVars (std::shared_ptr<Region> &out,std::size_t from,std::size_t to) {
    if (max_constraints.empty()) {
      out = std::shared_ptr<Region>(this);
      return;
    }
    // memoization
    if (from == 0 && to == integer_parts.size() / 2 - 1 && firstHalf) {
      // std::cout << "memoized" << std::endl;
      out = firstHalf;
      return;
    } else if (from == integer_parts.size() / 2 && to == integer_parts.size() - 1 && secondHalf) {
      // std::cout << "memoized" << std::endl;
      out = secondHalf;
      return;
    }
    // std::cout << "not memoized" << std::endl;
      
    out = std::make_shared<Region>();
    out->integer_parts = {integer_parts.begin () + from,integer_parts.begin() + to+1};
    out->max_constraints = {max_constraints.begin () + from,max_constraints.begin() + to+1};

    out->frac_order.clear();
    for (const auto &vec:frac_order) {
      std::list<int> newList;
      auto newBegin = std::lower_bound (vec.begin(), vec.end (),from);
      if (newBegin == vec.end ()) {
        break;
      }
      
      auto newEnd = std::lower_bound (vec.begin(), vec.end (),to);
      if (newEnd != vec.end ()) {
        newEnd++;
      }
      
      newList = {newBegin,newEnd};
      for_each (newList.begin (),newList.end (),[=] (int &v){v -= from;});
      out->frac_order.push_back (newList);
    }

    // memoization
    if (from == 0 && to == integer_parts.size() / 2 - 1) {
      firstHalf = out;
    } else if (from == integer_parts.size() / 2 && to == integer_parts.size() - 1) {
      secondHalf = out;
    }
  }
  inline std::tuple<std::vector< std::pair<int,int> >, std::list<std::list<int> >, std::vector<int> >
  toTuple () {
    return std::make_tuple(integer_parts,frac_order,max_constraints);
  }
private:
  std::shared_ptr<Region> firstHalf;
  std::shared_ptr<Region> secondHalf;
};

struct NFA {
  struct Edge {
    State source;
    State target;
    Alphabet c;
    inline bool operator == (const Edge e) const {
      return source == e.source && target == e.target && c == e.c;
    }
  };

  // This must be sorted.
  std::vector<State> initialStates;
  // Keys are source nodes.
  std::vector<std::vector<Edge> > edges;
  // This must be sorted.
  std::vector<State> acceptingStates;
  inline bool operator == (const NFA A) const {
    return initialStates == A.initialStates &&
      edges == A.edges &&
      acceptingStates == A.acceptingStates;
  }
};

struct RegionAutomaton : public NFA {
  std::vector<std::pair<State,Region> > regionStates;
  bool isPartialRun (std::vector<State>) const;
  bool isPartialRun2 (const RegionAutomaton &,const std::vector<State>&,const std::vector<State>&,const TAState);
};

std::ostream& operator << (std::ostream& os, const Region& r);

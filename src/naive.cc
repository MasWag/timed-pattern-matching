#include <iomanip>
#include <iostream>
#include <array>
#include <valarray>
#include <vector>
#include <utility>
#include <algorithm>
#include <functional>
#include <chrono>
#include <boost/program_options.hpp>

#include "types.hh"
#include "naive.hh"

#ifndef RUNNING_TEST

using namespace boost::program_options;

int main(int argc, char *argv[])
{
  // std::cin.tie(0);
  // std::ios::sync_with_stdio(false);
  std::vector<std::pair<Alphabet,double> > w;
  std::vector<Zone> ans;

  // visible options
  options_description visible("description of options");
  int testNum;
  int loopTimes;
  visible.add_options()
    ("help,h", "help")
    ("quiet,q", "quiet")
    ("test,t", value<int>(&testNum)->default_value(0),"number of test")
    ("loop,l", value<int>(&loopTimes)->default_value(1),"loop times");

  command_line_parser parser(argc, argv);
  parser.options (visible);
  variables_map vm;
  store(parser.run(), vm);
  notify(vm);

  if (vm.count("help")) {
    std::cout << argv[0] << " [option]... <input_file>...\n"
              << visible << std::endl;
    return 0;
  }

  // case 0
  constexpr int numOfVariables0 = 0;
  TimedAutomaton <numOfVariables0> TA0;
  TA0.initialStates = {0};
  TA0.edges = {
    {{0,1,'a',{},{}}},
    {{1,2,'b',{},{}}},
    {{2,3,'$',{},{}}},
    {}
  };
  
  TA0.acceptingStates = {3};
  TA0.max_constraints = {};

  // case 1
  constexpr int numOfVariables1 = 0;
  TimedAutomaton <numOfVariables1> TA1;
  TA1.initialStates = {0};
  TA1.edges = {
    {{0,1,'a',{},{}}},
    {{1,2,'b',{},{}}},
    {{2,1,'a',{},{}},
     {2,3,'$',{},{}}},
    {}
  };
  
  TA1.acceptingStates = {3};
  TA1.max_constraints = {};

  // case 2
  constexpr int numOfVariables2 = 2;
  TimedAutomaton <numOfVariables2> TA2;
  TA2.initialStates = {0};
  TA2.edges = {
    {{0,0,'a',{1},{}},
     {0,1,'a',{},{{ConstraintMaker(0) >= 1},
                  {ConstraintMaker(0) <= 1}}}},
    {{1,1,'a',{},{}},
     {1,2,'a',{},{{ConstraintMaker(1) >= 1},
                  {ConstraintMaker(1) <= 1}}}},
    {{2,3,'$',{},{}}},
    {}
  };
  TA2.acceptingStates = {3};
  TA2.max_constraints = {1,1};

  // case 3
  constexpr int numOfVariables3 = 3;
  TimedAutomaton <numOfVariables3> TA3;
  TA3.initialStates = {0};
  TA3.edges = {
    {{0,1,'p',{},{}},
     {0,2,'q',{},{}},
     {0,4,'$',{},{{ConstraintMaker(2) <= 80}}}},
    {{1,0,'r',{0},{{ConstraintMaker(0) <= 10}}},
     {1,2,'q',{},{}}},
    {{2,0,'s',{1},{{ConstraintMaker(1) <= 10}}},
     {2,3,'p',{},{}}},
    {{3,1,'s',{1},{{ConstraintMaker(1) <= 10}}},
     {3,2,'r',{0},{{ConstraintMaker(0) <= 10}}}},
    {}
  };
  TA3.acceptingStates = {4};
  TA3.max_constraints = {10,10,80};
  
  // case 4
  constexpr int numOfVariables4 = 1;
  TimedAutomaton <numOfVariables4> TA4;
  TA4.initialStates = {0};
  TA4.edges = {
    {{0,1,'a',{0},{{ConstraintMaker(0) > 1}}}},
    {{1,2,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{2,3,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{3,4,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{4,5,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{5,6,'a',{0},{{ConstraintMaker(0) > 1}}},
     {5,5,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{6,7,'$',{0},{{ConstraintMaker(0) <= 1},
                   {ConstraintMaker(0) >= 1}}},
     {6,6,'a',{0},{{ConstraintMaker(0) > 1}}}},
    {}
  };

  TA4.acceptingStates = {7};
  TA4.max_constraints = {1};

  // case 5
  constexpr int numOfVariables5 = 1;
  TimedAutomaton <numOfVariables5> TA5;
  TA5.initialStates = {0};
  TA5.edges = {
    {{0,1,'a',{0},{{ConstraintMaker(0) > 1}}}},
    // {{0,1,'b',{0},{}}},
    {{1,2,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{2,3,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{3,4,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{4,5,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{5,6,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{6,7,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{7,8,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{8,9,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{9,10,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{10,11,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{11,12,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{12,13,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{13,14,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{14,15,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{15,16,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{16,17,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{17,18,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{18,19,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{19,20,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{20,21,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{21,22,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{22,23,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{23,24,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{24,25,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{25,26,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{26,27,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{27,28,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{28,29,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{29,30,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{30,31,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{31,32,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{32,33,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{33,34,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{34,35,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{35,36,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{36,37,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{37,38,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{38,39,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{39,40,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{40,41,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{41,42,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{42,43,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{43,44,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{44,45,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{45,46,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{46,47,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{47,48,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{48,49,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{49,50,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{50,51,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{51,52,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{52,53,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{53,54,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{54,55,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{55,56,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{56,57,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{57,58,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{58,59,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{59,60,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{60,61,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{61,62,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{62,63,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{63,64,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{64,65,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{65,66,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{66,67,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{67,68,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{68,69,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{69,70,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{70,71,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{71,72,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{72,73,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{73,74,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{74,75,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{75,76,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{76,77,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{77,78,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{78,79,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{79,80,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{80,81,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{81,82,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{82,83,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{83,84,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{84,85,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{85,86,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{86,87,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{87,88,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{88,89,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{89,90,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{90,91,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{91,92,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{92,93,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{93,94,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{94,95,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{95,96,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{96,97,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{97,98,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{98,99,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{99,100,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{100,101,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{101,102,'a',{0},{{ConstraintMaker(0) > 1}}},
     {101,101,'a',{0},{{ConstraintMaker(0) < 1}}}},
    {{102,103,'$',{0},{{ConstraintMaker(0) <= 1},
                       {ConstraintMaker(0) >= 1}}},
     {102,102,'a',{0},{{ConstraintMaker(0) > 1}}}},
    {}};
  //   {{101,101,'a',{0},{{ConstraintMaker(0) < 1}}},
  //    {101,102,'b',{},{}}},
  //   {{102,103,'$',{},{}}},
  //   {}
  // };

  TA5.acceptingStates = {103};
  TA5.max_constraints = {1};

  // case 6
  constexpr int numOfVariables6 = 1;
  TimedAutomaton <numOfVariables6> TA6;
  TA6.initialStates = {0};
  TA6.edges = {
    {{0,1,'b',{0},{}}},
    {{1,2,'a',{},{{ConstraintMaker(0) < 1}}}},
    {{2,3,'a',{},{{ConstraintMaker(0) < 1}}}},
    {{3,4,'a',{},{{ConstraintMaker(0) < 1}}}},
    {{4,5,'a',{},{{ConstraintMaker(0) < 1}}}},
    {{5,6,'a',{},{{ConstraintMaker(0) > 1}}},
     {5,5,'a',{},{}}},
    {{6,7,'$',{},{}}},
    {}
  };

  TA6.acceptingStates = {7};
  TA6.max_constraints = {1};

  int N;
  std::cin >> N;
  w.resize (N);

  for (auto &p : w) {
    char c;
    double t;
    std::cin >> c >> t;
    p = std::make_pair (c,t);
  }

  for (int i = 0;i < loopTimes; i++ ) {
    std::cout << "precomputation: " << 0 << " ms" << std::endl;
    switch (testNum) {
    case 0:
      naive (w,TA0,ans);
      break;
    case 1:
      naive (w,TA1,ans);
      break;
    case 2:
      naive (w,TA2,ans);
      break;
    case 3:
      naive (w,TA3,ans);
      break;
    case 4:
      naive (w,TA4,ans);
      break;
    case 5:
      naive (w,TA5,ans);
      break;
    case 6:
      naive (w,TA6,ans);
      break;
    }
  }

  std::cout << ans.size() << " zones" << std::endl;

  if (vm.count ("quiet")) {
    return 0;
  }

  // print result
  std::cout << "Results" << std::endl;
  for (const auto &a : ans) {
    std::cout << a.lowerBeginConstraint.first <<std::setw(10)<< 
      (a.lowerBeginConstraint.second ? " <= " : " < ") << "t" << 
      (a.upperBeginConstraint.second ? " <= " : " < ") <<
      a.upperBeginConstraint.first << std::endl;
    std::cout << a.lowerEndConstraint.first << std::setw(10)<< 
      (a.lowerEndConstraint.second ? " <= " : " < ") << "t'" << 
      (a.upperEndConstraint.second ? " <= " : " < ") <<
      a.upperEndConstraint.first << std::endl;
    std::cout << a.lowerDeltaConstraint.first << std::setw(10)<< 
      (a.lowerDeltaConstraint.second ? " <= " : " < ") << "t' - t" << 
      (a.upperDeltaConstraint.second ? " <= " : " < ") <<
      a.upperDeltaConstraint.first << std::endl;
    std::cout << "=============================" << std::endl;
  }

  return 0;
}
#endif

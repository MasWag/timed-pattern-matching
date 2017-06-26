#include <cstdio>
#include <iostream>
#include <iomanip>
#include <array>
#include <valarray>
#include <vector>
#include <utility>
#include <algorithm>
#include <functional>
#include <chrono>
#include <boost/program_options.hpp>

#include "types.hh"
#include "naive_online.hh"

#ifndef RUNNING_TEST

using namespace boost::program_options;

int main(int argc, char *argv[])
{
  // visible options
  options_description visible("description of options");
  int testNum;
  int loopTimes;
  double resolution;
  std::string fileName;
  State length;
  bool isBinary = false;
  visible.add_options()
    ("help,h", "help")
    ("quiet,q", "quiet")
    ("test,t", value<int>(&testNum)->default_value(0),"number of test")
    ("loop,l", value<int>(&loopTimes)->default_value(1),"loop times")
    ("binary,b", "binary mode")
    ("resolution", value<double>(&resolution)->default_value(1.0),"resolution of clock (used in test case 3)")
    ("length", value<State>(&length)->default_value(1),"length of the pattern (used in test case 7)")
    ("input,i", value<std::string>(&fileName)->default_value("stdin"),"input file")
    ("printhashnum,p", "print number of calculated hashes");

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
  if (vm.count("binary")) {
    isBinary = true;
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
     {0,4,'$',{},{{ConstraintMaker(2) <= 80 * resolution}}}},
    {{1,0,'r',{0},{{ConstraintMaker(0) <= 10 * resolution}}},
     {1,2,'q',{},{}}},
    {{2,0,'s',{1},{{ConstraintMaker(1) <= 10 * resolution}}},
     {2,3,'p',{},{}}},
    {{3,1,'s',{1},{{ConstraintMaker(1) <= 10 * resolution}}},
     {3,2,'r',{0},{{ConstraintMaker(0) <= 10 * resolution}}}},
    {}
  };
  TA3.acceptingStates = {4};
  TA3.max_constraints = {int(10 * resolution) ,int(10 * resolution), int(80 * resolution)};

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

  TA6.max_constraints = {1};
  TA6.acceptingStates = {7};

  // case 7
  constexpr int numOfVariables7 = 1;
  TimedAutomaton <numOfVariables7> TA7;
  TA7.initialStates = {0};
  TA7.edges.resize(length + 4);  
  TA7.edges[0] = {{0,1,'a',{0},{{ConstraintMaker(0) > 1}}}};
  for (State e = 1; e <= length; e++) {
    TA7.edges[e] = {{e,e+1,'a',{0},{{ConstraintMaker(0) < 1}}}};
  }
  TA7.edges[length + 1] = {{1+length,2+length,'a',{0},{{ConstraintMaker(0) > 1}}},
                           {1+length,1+length,'a',{0},{{ConstraintMaker(0) < 1}}}};
  TA7.edges[length + 2] = {{2+length,3+length,'$',{0},{{ConstraintMaker(0) <= 1},
                                                       {ConstraintMaker(0) >= 1}}},
                           {2+length,2+length,'a',{0},{{ConstraintMaker(0) > 1}}}};
  TA7.acceptingStates = {length+3};
  TA7.max_constraints = {1};

  // case 8
  constexpr int numOfVariables8 = 1;
  TimedAutomaton <numOfVariables8> TA8;
  TA8.initialStates = {0};
  TA8.edges.resize(2 + length * 2);
  TA8.edges[0].reserve (length);
  for (State i = 0; i < length; i++) {
    TA8.edges[0].push_back({0,2 + 2 * i,'a',{0},{}});
    TA8.edges[2 + 2 * i] ={{2 + 2 * i,3 + 2 * i,'a',{0},{{{ConstraintMaker(0) < 1}}}}};
    TA8.edges[3 + 2 * i] ={{3 + 2 * i,1,'$',{},{{ConstraintMaker(0) < 1}}}};
  }

  TA8.max_constraints = {1};
  TA8.acceptingStates = {1};
#
  // case 9
  // The complex example in ICALP 2017
  constexpr int numOfVariables9 = 1;
  TimedAutomaton <numOfVariables9> TA9;
  TA9.initialStates = {0};
  // g_1 :: 0,1,2,3
  // g_2 :: 4,5,6,7
  // g_3 :: 8,9,10,11
  // g_4 :: 12,13,14,15
  // g_4, v <  v-, omega <  omega- :: 12
  // g_4, v <  v-, omega >= omega- :: 13
  // g_4, v >= v-, omega <  omega- :: 14
  // g_4, v >= v-, omega >= omega- :: 15
  TA9.edges = {
    {{0,1,'0',{0},{}},
     {0,1,'1',{0},{}},
     {0,1,'2',{0},{}},
     {0,1,'3',{0},{}}},
    {{1,2,'4',{},{{ConstraintMaker(0) < 10}}},
     {1,2,'5',{},{{ConstraintMaker(0) < 10}}},
     {1,2,'6',{},{{ConstraintMaker(0) < 10}}},
     {1,2,'7',{},{{ConstraintMaker(0) < 10}}}},
    {{2,2,'4',{},{{ConstraintMaker(0) < 10}}},
     {2,2,'5',{},{{ConstraintMaker(0) < 10}}},
     {2,2,'6',{},{{ConstraintMaker(0) < 10}}},
     {2,2,'7',{},{{ConstraintMaker(0) < 10}}},
     {2,3,'8',{},{{ConstraintMaker(0) < 10}}},
     {2,3,'9',{},{{ConstraintMaker(0) < 10}}},
     {2,3,'a',{},{{ConstraintMaker(0) < 10}}},
     {2,3,'b',{},{{ConstraintMaker(0) < 10}}}},
    {{3,3,'8',{},{{ConstraintMaker(0) < 10}}},
     {3,3,'9',{},{{ConstraintMaker(0) < 10}}},
     {3,3,'a',{},{{ConstraintMaker(0) < 10}}},
     {3,3,'b',{},{{ConstraintMaker(0) < 10}}},
     {3,4,'c',{0},{{ConstraintMaker(0) < 10}}},
     {3,4,'e',{0},{{ConstraintMaker(0) < 10}}},
     {3,5,'d',{},{{ConstraintMaker(0) < 10}}},
     {3,5,'f',{},{{ConstraintMaker(0) < 10}}}},
    {{4,4,'c',{},{{ConstraintMaker(0) <= 2}}},
     {4,4,'e',{},{{ConstraintMaker(0) <= 2}}},
     {4,5,'d',{},{{ConstraintMaker(0) <= 2}}},
     {4,5,'f',{},{{ConstraintMaker(0) <= 2}}}},
    {{5,6,'d',{0},{}}},
    {{6,7,'$',{},{{ConstraintMaker(0) > 1}}},
     {6,6,'d',{},{{ConstraintMaker(0) <= 1}}}},
    {}
  };

  TA9.acceptingStates = {7};
  TA9.max_constraints = {1};

  // case 10
  // g_1 :: 1
  // g_2 :: 2
  // g_3 :: 3
  // g_4 :: 4
  // omega >= omega- :: 5
  // omega <  omega- :: 6
  // v >= v- :: 7
  // v < v-  :: 8
  TimedAutomaton <1> Phi8;
  Phi8.initialStates = {9};
  Phi8.edges = {{},{{1,2,'5',{},{}},{1,3,'2',{},{}}},{{2,4,'2',{},{}}},{{3,4,'5',{},{}},{3,5,'3',{},{}}},{{4,6,'3',{},{}}},{{5,6,'5',{},{}},{5,7,'4',{0},{ {ConstraintMaker{0} <= 10} }}},{{6,8,'4',{0},{ {ConstraintMaker{0} <= 10} }}},{{7,8,'5',{},{}}},{{8,0,'$',{},{ {ConstraintMaker{0} > 1} }}},{{9,1,'1',{},{}},{9,10,'5',{},{}}},{{10,2,'1',{},{}}}};
  Phi8.acceptingStates = {0};
  Phi8.max_constraints = {10};

  // case 11
  // g_1 :: 1
  // g_2 :: 2
  // g_3 :: 3
  // g_4 :: 4
  TimedAutomaton <1>  Phi5;
  Phi5.initialStates = {9};
  Phi5.edges = {{},{{1,5,'2',{},{ {ConstraintMaker{0} < 2} }},{1,5,'3',{},{ {ConstraintMaker{0} < 2} }},{1,5,'4',{},{ {ConstraintMaker{0} < 2} }}},{{2,6,'1',{},{ {ConstraintMaker{0} < 2} }},{2,6,'3',{},{ {ConstraintMaker{0} < 2} }},{2,6,'4',{},{ {ConstraintMaker{0} < 2} }}},{{3,7,'2',{},{ {ConstraintMaker{0} < 2} }},{3,7,'1',{},{ {ConstraintMaker{0} < 2} }},{3,7,'4',{},{ {ConstraintMaker{0} < 2} }}},{{4,8,'2',{},{ {ConstraintMaker{0} < 2} }},{4,8,'3',{},{ {ConstraintMaker{0} < 2} }},{4,8,'1',{},{ {ConstraintMaker{0} < 2} }}},{{5,0,'$',{},{}}},{{6,0,'$',{},{}}},{{7,0,'$',{},{}}},{{8,0,'$',{},{}}},{{9,1,'1',{0},{}},{9,2,'2',{0},{}},{9,3,'3',{0},{}},{9,4,'4',{0},{}}}};
  Phi5.acceptingStates = {0};
  Phi5.max_constraints = {2};

  // case 12
  // g_1 :: 1
  // g_2 :: 2
  // g_3 :: 3
  // g_4 :: 4
  TimedAutomaton <1> Phi4;
  Phi4.initialStates = {3};
  Phi4.edges = {{},{{1,2,'2',{},{ {ConstraintMaker{0} < 2} }}},{{2,0,'$',{},{}}},{{3,1,'1',{0},{}}}};
  Phi4.acceptingStates = {0};
  Phi4.max_constraints = {2};

  // case 13
  // 0,1 mode is normal
  // 2,3 mode is power
  // 0,2 not settled
  // 1,3 settled
  TimedAutomaton <1> hscc2014;
  hscc2014.initialStates = {2};
  hscc2014.edges = {{},{{1,3,'0',{},{ {ConstraintMaker{0} < 100} }}},{{2,1,'1',{0},{}}},{{3,0,'$',{},{}}}};
  hscc2014.acceptingStates = {0};
  hscc2014.max_constraints = {100};

  // case 14
  // 0,1 mode is normal
  // 2,3 mode is power
  // 0,2 not settled
  // 1,3 settled
  TimedAutomaton <1> hscc2014_2;
  hscc2014_2.initialStates = {2};
  hscc2014_2.edges = {{},{{1,3,'0',{},{}}},{{2,1,'1',{0},{}}},{{3,0,'$',{},{ {ConstraintMaker{0} > 100} }}}};
  hscc2014_2.acceptingStates = {0};
  hscc2014_2.max_constraints = {100};

  int N;
  FILE* file = stdin;
  if (fileName != "stdin") {
    file = fopen(fileName.c_str(), "r");
  }
  if (isBinary) {
    fread(&N, sizeof(int), 1, file);
  } else {
    fscanf(file, "%d", &N);
  }
  
#ifdef LAZY_READ
  WordLazyDeque<std::pair<Alphabet,double> > w(N, file, isBinary);
  AnsNum<ansZone> ans;
#else
  WordVector<std::pair<Alphabet,double> > w(N, file, isBinary);
  AnsVec<ansZone> ans;
#endif

  for (int i = 0;i < loopTimes; i++ ) {
    switch (testNum) {
    case 0:
      naiveOnline (w,TA0,ans);
      break;
    case 1:
      naiveOnline (w,TA1,ans);
      break;
    case 2:
      naiveOnline (w,TA2,ans);
      break;
    case 3:
      naiveOnline (w,TA3,ans);
      break;
    case 4:
      naiveOnline (w,TA4,ans);
      break;
    case 5:
      naiveOnline (w,TA5,ans);
      break;
    case 6:
      naiveOnline (w,TA6,ans);
      break;
    case 7:
      naiveOnline (w,TA7,ans);
      break;
    case 8:
      naiveOnline (w,TA8,ans);
      break;
    case 9:
      naiveOnline (w,TA9,ans);
      break;
    case 10:
      naiveOnline (w,Phi8,ans);
      break;
    case 11:
      naiveOnline (w,Phi5,ans);
      break;
    case 12:
      naiveOnline (w,Phi4,ans);
      break;
    case 13:
      naiveOnline (w,hscc2014,ans);
      break;
    case 14:
      naiveOnline (w,hscc2014_2,ans);
      break;
    }
  }


  std::cout << ans.size() << " zones" << std::endl;

  if (vm.count("printhashnum")) {
    switch (testNum) {
    case 0:
      std::cout << "/" << TA0.edges.size() << " hash calculated" << std::endl;    
      break;
    case 1:
      std::cout << "/" << TA1.edges.size() << " hash calculated" << std::endl;    
      break;
    case 2:
      std::cout << "/" << TA2.edges.size() << " hash calculated" << std::endl;    
      break;
    case 3:
      std::cout << "/" << TA3.edges.size() << " hash calculated" << std::endl;    
      break;
    case 4:
      std::cout << "/" << TA4.edges.size() << " hash calculated" << std::endl;    
      break;
    case 5:
      std::cout << "/" << TA5.edges.size() << " hash calculated" << std::endl;    
      break;
    }
  }
  
  if (vm.count ("quiet")) {
    return 0;
  }

#ifndef LAZY_READ
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
#endif

  return 0;
}
#endif

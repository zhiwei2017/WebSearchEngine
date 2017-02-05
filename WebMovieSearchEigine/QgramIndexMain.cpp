// Copyright 2015, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Zhiwei Zhang <zhang>.

#include <string>
#include <iostream>
#include "./QgramIndex.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage ./InvertedIndexMain <file>\n";
    exit(1);
  }
  string fileName = argv[1];
  QgramIndex q(3);
  q.readFromFile(fileName);
  std::cout << "(If you want to quit this search program, please input 'quit')" << std::endl;
  while (true) {
    // get the query from input.
    string query;
    std::cout << "Please input your query keywords: ";
    getline(std::cin, query);
    if (query == "quit" || query == "q") {
      std::cout << "Do you want to quit this search program? (YES/NO) : ";
      string temp;
      getline(std::cin, temp);
      string answer = q.getLowercase(temp);
      if (answer == "yes" || answer == "y") {
        break;
      }
    }
    string prefix = q.split(query);
    Recordlist res = q.findMatches(prefix, prefix.size()/4, 10, true, true);
    for (size_t i = 0; i < res.size(); ++i) {
      std::cout << res[i]._Name << " (" << res[i]._Year << ")" << std::endl;
    }
  }
  return 0;
}

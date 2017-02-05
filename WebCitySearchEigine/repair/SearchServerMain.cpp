// Copyright 2015, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Björn Buchhold <buchholb>.
#define EMPH_ON  "\033[1m"
#define EMPH_OFF "\033[21m"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include "./SearchServer.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::flush;
using std::cerr;

/**
 * Demo search server
 *
 * IMPORTANT: For the exercise sheet, please adhere to the following:
 *
 *  1.  The main program should be called SearchServerMain
 *  2.  It should take exactly TWO arguments: <file> <port>, where
 *      <file> is the full path to the file with the movie records,
 *      and <port> is the port on which the server is listening.
 *
 */
int main(int argc, char** argv) {
  if (argc != 4) {
    std::cout << "Usage: SearchServerMain <inputfile> <outputfile> <port>" << std::endl;
    exit(1);
  }
  string inputFile(argv[1]);
  string outputFile(argv[2]);
  uint16_t port = atoi(argv[3]);
  optind = 1;
  SearchServer server(inputFile, outputFile, port);
  server.run();
  return 0;
}

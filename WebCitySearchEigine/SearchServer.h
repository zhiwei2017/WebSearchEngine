// Copyright 2015, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Zhiwei Zhang <zhang>

#ifndef SHEET_07_SEARCHSERVER_H_
#define SHEET_07_SEARCHSERVER_H_

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "./QgramIndex.h"

using std::string;
using std::vector;

class SearchServer {
 public:
  explicit SearchServer(const string& filename, uint16_t port);
  void run();

 private:
  string getDummyResult(const string& query, size_t hit = 10);
  string getHttpResponse(const string& request);
  static string escapeForJson(const string& orig);

  QgramIndex _q;
  uint16_t _port;
  boost::asio::io_service _io_service;
  boost::asio::ip::tcp::acceptor _acceptor;

  // UTILITY STUFF (should actually be in its own class):

  // Safe startWith function. Returns true iff prefix is a
  // prefix of text. Using a larger pattern than text.size()
  // will return false. Case sensitive.
  static bool startsWith(const string& text, const char* prefix,
      size_t patternSize);
  static bool startsWith(const string& text, const string& prefix);
  static bool startsWith(const string& text, const char* prefix);

  // Safe endsWith function. Returns true iff suffix is a
  // prefix of text. Using a larger pattern than text.size()
  // will return false. Case sensitive.
  static bool endsWith(const string& text, const char* suffix,
      size_t patternSize);
  static bool endsWith(const string& text, const string& suffix);
  static bool endsWith(const string& text, const char* suffix);

  // Splits a string at the separator, kinda like python.
  static vector<string> split(const string& orig, const char sep);

  // Replace the %20 als ' '.
  static string decodeFromHttp(const string& encoded);
};

#endif  // SHEET_07_SEARCHSERVER_H_

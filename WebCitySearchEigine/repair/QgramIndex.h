// Copyright 2015, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Zhiwei Zhang <zhang>.

#ifndef SHEET_07_QGRAMINDEX_H_
#define SHEET_07_QGRAMINDEX_H_

#include <codecvt>
#include <gtest/gtest.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <set>
#include "./Record.h"

using std::string;
using std::wstring;
using std::vector;
using std::set;

typedef set<wstring> Qgram;
typedef vector<vector<size_t>> RecordIDlist;
typedef vector<Record> Recordlist;
typedef vector<Posting> Postinglist;
typedef std::unordered_map<wstring, vector<size_t>> IndexMap;
typedef std::codecvt_utf8<wchar_t> convert_typeX;

// A simple inverted index, as explained in the lecture.
class QgramIndex {
 public:

  explicit QgramIndex(unsigned char q = 3) {
    _q = q;
  }
  // Constructs inverted index from given file.
  // The format is one record per line.
  void readFromFile(const string& fileName);
  FRIEND_TEST(QgramIndexTest, readFromFile);

  // calculate the qgrams
  Qgram qgrams(const wstring& records);

  // merge a lot of lists into one
  Postinglist merge(const RecordIDlist& recordIDlists);

  // compute the PED, if delta = -1, means delta is unknown,
  size_t computePed(const wstring& p, const wstring& s, const int& delta);

  // find the matches, and parameter show controlls to show the runtime.
  Recordlist findMatches(const wstring& prefix, const size_t& delta,
                    const size_t& k = 5,
                    const bool& useQindex = true);

  inline static wstring s2ws(const string& str);
  inline static string ws2s(const wstring& wstr);

  // The the lowercase version of a string or wstring.
  template<typename T>
  static T getLowercase(const T& orig) {
    T retVal;
    retVal.reserve(orig.size());
    for (size_t i = 0; i < orig.size(); ++i) {
      retVal += tolower(orig[i]);
    }
    return retVal;
  }

  // split the space between a string or wstring
  template<typename T>
  static T split(const T& orig) {
    T result;
    size_t i = 0;
    size_t start = 0;

    while (i < orig.size()) {
      if (orig[i] == 32) {
        result += getLowercase(orig.substr(start, i - start));
        start = i + 1;
      }
      ++i;
    }
    result += getLowercase(orig.substr(start, i - start));
    return result;
  }

 private:

  // strore the qgrams and its records
  IndexMap _invertedLists;

  unsigned char _q;

  // The text of all the documents, indexed by document id. Since document ids
  // are consecutive, starting at 0, a simple array suffices.
  Recordlist _documents;
};

// ____________________________________________________________________________
wstring QgramIndex::s2ws(const std::string& str) {
  std::wstring_convert<convert_typeX, wchar_t> converterX;
  return converterX.from_bytes(str);
}

// ____________________________________________________________________________
string QgramIndex::ws2s(const std::wstring& wstr) {
  std::wstring_convert<convert_typeX, wchar_t> converterX;
  return converterX.to_bytes(wstr);
}
#endif  // SHEET_07_QGRAMINDEX_H_

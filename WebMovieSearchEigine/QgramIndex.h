// Copyright 2015, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Zhiwei Zhang <zhang>.

#ifndef SHEET_06_QGRAMINDEX_H_
#define SHEET_06_QGRAMINDEX_H_

#include <gtest/gtest.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <set>
#include "./Record.h"

using std::string;
using std::vector;
using std::set;

typedef set<string> Qgram;
typedef vector<vector<size_t>> RecordIDlist;
typedef vector<Record> Recordlist;
typedef vector<Posting> Postinglist;
typedef std::unordered_map<string, vector<size_t>> IndexMap;

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
  Qgram qgrams(string records);

  // The the lowercase version of a string.
  inline static string getLowercase(const string& orig);

  // split the string.
  inline static string split(const string& orig);

  // merge a lot of lists into one
  Postinglist merge(const RecordIDlist& recordIDlists);

  // compute the PED, if delta = -1, means delta is unknown,
  size_t computePed(const string& p, const string& s, const int& delta);

  // find the matches, and parameter show controlls to show the runtime.
  Recordlist findMatches(const string& prefix, const size_t& delta,
                    const size_t& k = 5,
                    const bool& useQindex = true,
                    const bool& show = false);

 private:

  // strore the qgrams and its records
  IndexMap _invertedLists;

  unsigned char _q;

  // The text of all the documents, indexed by document id. Since document ids
  // are consecutive, starting at 0, a simple array suffices.
  Recordlist _documents;
};

// Inline methods
// ____________________________________________________________________________
string QgramIndex::getLowercase(const string& orig) {
  string retVal;
  retVal.reserve(orig.size());
  for (size_t i = 0; i < orig.size(); ++i) {
    retVal += tolower(orig[i]);
  }
  return retVal;
}

// _____________________________________________________________________________
string QgramIndex::split(const string& orig) {
  string result;
  size_t i = 0;
  size_t start = 0;

  while (i < orig.size()) {
    // The  > 0 so that we don't split at non-ascii chars.
    // It is very important that one can search for "björn" :-).
    if (orig[i] > 0 && !isalnum(orig[i])) {
      if (i > start) {
        result += getLowercase(orig.substr(start, i - start));
      }
      start = i + 1;
    }
    ++i;
  }

  // if the last char is not space or non-alnum, we must also take the word
  // into the list.
  if (i > start) {
    result += getLowercase(orig.substr(start, i - start));
  }
  return result;
}

#endif  // SHEET_06_QGRAMINDEX_H_

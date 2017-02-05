// Copyright 2015, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Zhiwei Zhang <zhang>.

#ifndef SHEET_07_RECORD_H_
#define SHEET_07_RECORD_H_

#include <string>

using std::string;
using std::wstring;

class Record {
 public:
  Record(wstring City, string Country, string Union, size_t Population) {
    _City = City;
    _Country = Country;
    _Union= Union;
    _Population = Population;
  }

  wstring _City;
  string _Country;
  string _Union;
  size_t _Population;
};

class Posting {
  friend class QgramIndex;
  friend class Comparator;
 public:
  Posting(size_t RecordID, size_t Cross, size_t Population) {
    _RecordID = RecordID;
    _Cross = Cross;
    _Population = Population;
  }
 private:
  size_t _RecordID;
  size_t _Cross;
  size_t _Population;
};

class Comparator {
 public:
  bool operator() (const Posting& i, const Posting& j) const {
    if (i._Cross == j._Cross) {
      return i._Population > j._Population;
    }
    return i._Cross > j._Cross;
  }
};
#endif  // SHEET_07_RECORD_H_

// Copyright 2015, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Zhiwei Zhang <zhang>.

#ifndef SHEET_06_RECORD_H_
#define SHEET_06_RECORD_H_

#include <string>

using std::string;

class Record {
 public:
  Record(string MID, string Name, string Year, size_t Popularity) {
    _MID = MID;
    _Name = Name;
    _Year= Year;
    _Popularity = Popularity;
  }
  string _MID;
  string _Name;
  string _Year;
  size_t _Popularity;
};

class Posting {
  friend class QgramIndex;
  friend class Comparator;
 public:
  Posting(size_t RecordID, size_t Cross, size_t Popularity) {
    _RecordID = RecordID;
    _Cross = Cross;
    _Popularity = Popularity;
  }
 private:
  size_t _RecordID;
  size_t _Cross;
  size_t _Popularity;
};

class Comparator {
 public:
  bool operator() (const Posting& i, const Posting& j) const {
    if (i._Cross == j._Cross) {
      return i._Popularity > j._Popularity;
    }
    return i._Cross > j._Cross;
  }
};
#endif  // SHEET_06_RECORD_H_

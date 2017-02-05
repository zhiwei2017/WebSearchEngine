// Copyright 2015, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Zhiwei Zhang <zhang>.

#include <stdio.h>
#include <unordered_set>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include "./QgramIndex.h"

using std::vector;

// _____________________________________________________________________________
void QgramIndex::readFromFile(const string& fileName) {
  // initialize the date.
  _invertedLists.clear();
  _documents.clear();
  _invertedLists.reserve(2000000);
  _documents.reserve(2000000);

  // the old readFromFile function.
  // read the words from the given file
  string line;
  std::ifstream in(fileName.c_str());
  size_t RecordId = 0;

  // now we gonna read the file line by line.
  while (std::getline(in, line)) {
    size_t i = 0;
    size_t start = 0;
    size_t s = 0;
    wstring City;
    string Country;
    string Union;
    size_t Population = 0;

    while (i < line.size()) {
      if (line[i] == 9) {
        if (s == 0) {
          City = s2ws(line.substr(start, i - start));
          ++s;
        } else if (s == 1) {
          Country = line.substr(start, i - start);
          ++s;
        } else if (s == 2) {
          Union = line.substr(start, i - start);
          ++s;
        } else {
          Population = stoull(line.substr(start, i - start));
        }
        start = i + 1;
      }
      ++i;
    }
    if (i > start && s == 3) {
      Population = stoull(line.substr(start, i - start));
    }
    for (auto qgram : qgrams(split(City))) {
      if (qgram.size() > 0) {
        _invertedLists[qgram].push_back(RecordId);
      }
    }
    _documents.push_back(Record(City, Country, Union, Population));
    RecordId++;
  }
}

// _____________________________________________________________________________
Qgram QgramIndex::qgrams(const wstring& records) {
  Qgram result;
  wstring pad = wstring(_q - 1, '$');
  pad += records;
  for (size_t i = 0; i < pad.size() - _q + 1; ++i) {
    result.insert(pad.substr(i, _q));
  }
  return result;
}

// _____________________________________________________________________________
Postinglist QgramIndex::merge(const RecordIDlist& recordIDlists) {
  Postinglist result;
  result.reserve(recordIDlists[0].size());
  for (auto record : recordIDlists[0]) {
    result.push_back(Posting(record, 1, _documents[record]._Population));
  }
  for (size_t i = 1; i < recordIDlists.size(); ++i) {
    size_t a = 0;
    size_t b = 0;
    while (a < result.size() && b < recordIDlists[i].size()) {
      if (result[a]._RecordID == recordIDlists[i][b]) {
        ++result[a]._Cross;
        ++a;
        ++b;
      } else if (result[a]._RecordID < recordIDlists[i][b]) {
        while (result[a]._RecordID < recordIDlists[i][b] && a < result.size()) {
          ++a;
        }
      } else if (result[a]._RecordID > recordIDlists[i][b]) {
        while (result[a]._RecordID > recordIDlists[i][b]
               && b < recordIDlists[i].size()) {
          result.insert(result.begin() + a, Posting(recordIDlists[i][b], 1,
                _documents[recordIDlists[i][b]]._Population));
          ++a;
          ++b;
        }
      }
      while (a == result.size() && b < recordIDlists[i].size()) {
        result.insert(result.end(), Posting(recordIDlists[i][b], 1,
              _documents[recordIDlists[i][b]]._Population));
        break;
      }
    }
  }
  std::sort(result.begin(), result.end(), Comparator());
  return result;
}

// _____________________________________________________________________________
size_t QgramIndex::computePed(const wstring& p, const wstring& s,
                              const int& delta) {
  size_t n = p.size() + 1;
  size_t m = std::min(p.size() + delta + 1, s.size() + 1);
  size_t matrix[n][m];
  for (size_t i = 0; i < m; ++i) matrix[0][i] = i;
  for (size_t i = 0; i < n; ++i) matrix[i][0] = i;
  for (size_t i = 1; i < n; ++i) {
    for (size_t j = 1; j < m; ++j) {
      size_t repl = (p[i - 1] == s[j - 1] ?
                     matrix[i - 1][j - 1] : matrix[i - 1][j - 1] + 1);
      matrix[i][j] = std::min(std::min(repl, matrix[i][j - 1] + 1),
                              matrix[i - 1][j] + 1);
    }
  }
  size_t result = matrix[n - 1][0];
  for (size_t i = 1; i < m; ++i) {
    result = (matrix[n - 1][i] < result ? matrix[n - 1][i] : result);
  }
  return result;
}

// _____________________________________________________________________________
Recordlist QgramIndex::findMatches(const wstring& prefix, const size_t& delta,
          const size_t& k, const bool& useQindex) {
  size_t numberComputation = 0;
  Recordlist records;
  records.reserve(k);

  if (useQindex) {
    Qgram qgramOfPrefix = qgrams(prefix);
    RecordIDlist recordIDlists;
    recordIDlists.reserve(qgramOfPrefix.size());
    for (auto qgram : qgramOfPrefix) {
      if (_invertedLists.count(qgram) != 0) {
        recordIDlists.push_back(_invertedLists[qgram]);
      }
    }

    if (recordIDlists.size() == 0) {
      return records;
    }

    Postinglist matches = merge(recordIDlists);
    size_t threshold = prefix.size() - _q * delta;
    for (auto posting : matches) {
      if (posting._Cross >= threshold) {
        size_t ped = computePed(prefix,
            split(_documents[posting._RecordID]._City), delta);
        ++numberComputation;
        if (ped <= delta) {
          records.push_back(_documents[posting._RecordID]);
          if (records.size() == k) {
            break;
          }
        }
      }
    }
  } else {
    for (size_t i = 0; i < _documents.size(); ++i) {
      size_t ped = computePed(prefix, split(_documents[i]._City), delta);
      ++numberComputation;
      if (ped <=  delta) {
        records.push_back(_documents[i]);
        if (records.size() == k) {
          break;
        }
      }
    }
  }

  return records;
}

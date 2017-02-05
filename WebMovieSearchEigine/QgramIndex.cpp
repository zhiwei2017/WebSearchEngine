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
#include "./Record.h"
#include "./QgramIndex.h"
#include "./Timer.h"

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
    string Name = "";
    string words = "";
    string MID = "m/";
    string Year = "";
    size_t Popularity = 0;
    size_t nameStart = 0;

    while (i < line.size()) {
      if (line[i] > 0 && !isalnum(line[i])) {
        if (i > start) {
          if (s == 0) {
            if (line[i] == 9) {
              MID += line.substr(start, i - start);
              ++s;
              nameStart = i + 1;
            }
          } else if (s == 1) {
            if (line[i] != 9) {
              words += line.substr(start, i - start);
            } else {
              Name += line.substr(nameStart, i - nameStart);
              words += line.substr(start, i - start);
              words = QgramIndex::getLowercase(words);
              ++s;
            }
          } else if (s == 2) {
            if (line[i] == 9) {
              Year = line.substr(start, i - start);
              ++s;
            }
          } else {
            Popularity = stoull(line.substr(start, i - start));
          }
        }
        start = i + 1;
      }
      ++i;
    }
    if (i > start && s == 3) {
      Popularity = stoull(line.substr(start, i - start));
    }
    for (auto qgram : qgrams(words)) {
      if (qgram.size() > 0) {
        _invertedLists[qgram].push_back(RecordId);
      }
    }
    _documents.push_back(Record(MID, Name, Year, Popularity));
    RecordId++;
  }
}

// _____________________________________________________________________________
Qgram QgramIndex::qgrams(string records) {
  Qgram result;
  string pad = string(_q - 1, '$');
  records = pad + records;
  for (size_t i = 0; i < records.size() - _q + 1; ++i) {
    result.insert(records.substr(i, _q));
  }
  return result;
}

// _____________________________________________________________________________
Postinglist QgramIndex::merge(const RecordIDlist& recordIDlists) {
  Postinglist result;
  result.reserve(recordIDlists[0].size());
  for (auto record : recordIDlists[0]) {
    result.push_back(Posting(record, 1, _documents[record]._Popularity));
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
                _documents[recordIDlists[i][b]]._Popularity));
          ++a;
          ++b;
        }
      }
      while (a == result.size() && b < recordIDlists[i].size()) {
        result.insert(result.end(), Posting(recordIDlists[i][b], 1,
              _documents[recordIDlists[i][b]]._Popularity));
        break;
      }
    }
  }
  std::sort(result.begin(), result.end(), Comparator());
  return result;
}

// _____________________________________________________________________________
size_t QgramIndex::computePed(const string& p, const string& s,
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
Recordlist QgramIndex::findMatches(const string& prefix, const size_t& delta,
          const size_t& k, const bool& useQindex, const bool& show) {
  Timer t;
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
      t.stop();
      if (show) {
        std::cout << t.getUSecs() << "\t" << numberComputation << std::endl;
      }
      return records;
    }

    Postinglist matches = merge(recordIDlists);
    size_t threshold = prefix.size() - _q * delta;
    t.start();
    for (auto posting : matches) {
      if (posting._Cross >= threshold) {
        size_t ped = computePed(prefix,
            split(_documents[posting._RecordID]._Name), delta);
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
    t.start();
    for (size_t i = 0; i < _documents.size(); ++i) {
      size_t ped = computePed(prefix, split(_documents[i]._Name), delta);
      ++numberComputation;
      if (ped <=  delta) {
        records.push_back(_documents[i]);
        if (records.size() == k) {
          break;
        }
      }
    }
  }

  t.stop();
  if (show) {
    std::cout << t.getUSecs() << "\t" << numberComputation << std::endl;
  }
  return records;
}

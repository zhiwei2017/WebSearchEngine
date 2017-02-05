// Copyright 2015, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Zhiwei Zhang <zhang>.

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "./QgramIndex.h"

// _____________________________________________________________________________
TEST(QgramIndexTest, readFromFile) {
  QgramIndex q;
  q.readFromFile("example.txt");
  ASSERT_EQ(6, q._invertedLists.size());
  ASSERT_EQ(1, q._invertedLists[L"$$ö"].size());
  ASSERT_EQ(0, q._invertedLists[L"$$ö"][0]);
  ASSERT_EQ(1, q._invertedLists[L"$öä"].size());
  ASSERT_EQ(0, q._invertedLists[L"$öä"][0]);
  ASSERT_EQ(1, q._invertedLists[L"öäü"].size());
  ASSERT_EQ(0, q._invertedLists[L"öäü"][0]);
  ASSERT_EQ(0, q._invertedLists[L"äüä"][0]);
  ASSERT_EQ(1, q._invertedLists[L"äüä"][1]);
  ASSERT_EQ(2, q._invertedLists[L"äüä"].size());
  ASSERT_EQ(1, q._invertedLists[L"$$ä"][0]);
  ASSERT_EQ(1, q._invertedLists[L"$$ä"].size());
  ASSERT_EQ(1, q._invertedLists[L"$äü"][0]);
  ASSERT_EQ(1, q._invertedLists[L"$äü"].size());
}

// _____________________________________________________________________________
TEST(QgramIndexTest, qgrams) {
  wstring p = L"öäüä";
  QgramIndex q;
  Qgram qg = q.qgrams(p);
  ASSERT_EQ(1, qg.count(L"$$ö"));
  ASSERT_EQ(1, qg.count(L"$öä"));
  ASSERT_EQ(1, qg.count(L"öäü"));
  ASSERT_EQ(1, qg.count(L"äüä"));
  ASSERT_EQ(0, qg.count(L"$$ä"));
}

// _____________________________________________________________________________
TEST(QgramIndexTest, computePed) {
  QgramIndex q;
  ASSERT_EQ(2, q.computePed(L"shwartz", L"schwarzenegger", 2));
  ASSERT_EQ(2, q.computePed(L"matriks", L"matrix", 2));
}

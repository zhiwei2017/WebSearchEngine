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
  ASSERT_EQ(1, q._invertedLists["$$b"].size());
  ASSERT_EQ(0, q._invertedLists["$$b"][0]);
  ASSERT_EQ(1, q._invertedLists["$ba"].size());
  ASSERT_EQ(0, q._invertedLists["$ba"][0]);
  ASSERT_EQ(1, q._invertedLists["ban"].size());
  ASSERT_EQ(0, q._invertedLists["ban"][0]);
  ASSERT_EQ(0, q._invertedLists["ana"][0]);
  ASSERT_EQ(1, q._invertedLists["ana"][1]);
  ASSERT_EQ(2, q._invertedLists["ana"].size());
  ASSERT_EQ(1, q._invertedLists["$$a"][0]);
  ASSERT_EQ(1, q._invertedLists["$$a"].size());
  ASSERT_EQ(1, q._invertedLists["$an"][0]);
  ASSERT_EQ(1, q._invertedLists["$an"].size());
}

// _____________________________________________________________________________
TEST(QgramIndexTest, qgrams) {
  string p = "bana";
  QgramIndex q;
  Qgram qg = q.qgrams(p);
  ASSERT_EQ(1, qg.count("$$b"));
  ASSERT_EQ(1, qg.count("$ba"));
  ASSERT_EQ(1, qg.count("ban"));
  ASSERT_EQ(1, qg.count("ana"));
  ASSERT_EQ(0, qg.count("$$a"));
}

// _____________________________________________________________________________
TEST(QgramIndexTest, computePed) {
  QgramIndex q;
  ASSERT_EQ(2, q.computePed("shwartz", "schwarzenegger", 2));
  ASSERT_EQ(2, q.computePed("matriks", "matrix", 2));
}

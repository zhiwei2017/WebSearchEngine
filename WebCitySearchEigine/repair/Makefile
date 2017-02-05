BOOST_STUFF = -lboost_system -lboost_thread -lpthread 
CXX = g++ -O3 -Wall -std=c++0x
MAIN_BINARIES = $(basename $(wildcard *Main.cpp))
TEST_BINARIES = $(basename $(wildcard *Test.cpp))
HEADER = $(wildcard *.h)
OBJECTS = $(addsuffix .o, $(basename $(filter-out %Main.cpp %Test.cpp, $(wildcard *.cpp))))
CPPLINT_PATH = ../cpplint.py
PORT = 9999
INPUT_FILE = cities-eu.corrupted.txt
OUTPUT_FILE = cities-eu.txt

.PRECIOUS: %.o

all: 
	$(MAKE) compile 
	$(MAKE) test 
	$(MAKE) checkstyle

compile: $(MAIN_BINARIES) $(TEST_BINARIES)

test: $(TEST_BINARIES)
	for T in $(TEST_BINARIES); do ./$$T; done

checkstyle:
	python $(CPPLINT_PATH) *.h *.cpp

clean:
	rm -f *.o
	rm -f $(MAIN_BINARIES)
	rm -f $(TEST_BINARIES)
	rm -f *.class
	rm -f *Test.TMP.*
	rm -f core

%Main: %Main.cpp $(OBJECTS)
	$(CXX) -o $@ $^ $(BOOST_STUFF)

%Test: %Test.cpp $(OBJECTS)
	$(CXX) -o $@ $^ -lgtest -lgtest_main -lpthread $(BOOST_STUFF) 

%.o: %.cpp $(HEADER)
	$(CXX) -c $< 

start:
	./SearchServerMain $(INPUT_FILE) $(OUTPUT_FILE) $(PORT)

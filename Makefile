CXX = g++
CXXFLAGS = -std=c++11 -lsnappy -O3 -march=native -Wall -funroll-loops -Wno-unused-result

all: snappycat

snappycat: src/snappycat.cpp
		$(CXX) src/snappycat.cpp -o snappycat $(CXXFLAGS)

clean:
		rm -rf snappycat

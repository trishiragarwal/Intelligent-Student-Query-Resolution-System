CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude

all: query_system test_suite

query_system: src/main.cpp include/*.h
	$(CXX) $(CXXFLAGS) src/main.cpp -o query_system

test_suite: tests/test_suite.cpp include/*.h
	$(CXX) $(CXXFLAGS) tests/test_suite.cpp -o test_suite

run: query_system
	./query_system

test: test_suite
	./test_suite

clean:
	rm -f query_system test_suite query_system.exe test_suite.exe

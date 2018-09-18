CPPFLAGS= -std=c++0x -Wall -O0 -g3
LDFLAGS= -L$(GTEST_DIR)/make 
GTEST_DIR=./googletest/googletest


%.o: %.cpp
	g++ $(CPPFLAGS) -I$(GTEST_DIR)/include -c $< -o $@

swingTest: swingTest.o
	g++ $(LDFLAGS) -o $@ swingTest.o libgtest.a -pthread

googleTest:
	g++ -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) \
	-pthread -c $(GTEST_DIR)/src/gtest_main.cc
	ar -rv libgtest.a gtest_main.o

clean:
	rm *.o swingTest

CXX = g++
CXXFLAGS = -Wall -g

driver: dtree.o utree.o mytest.cpp
	$(CXX) $(CXXFLAGS) dtree.o utree.o mytest.cpp -o driver

dtree.o: dtree.h dtree.cpp
	$(CXX) $(CXXFLAGS) -c dtree.cpp

utree.o: utree.h utree.cpp
	$(CXX) $(CXXFLAGS) -c utree.cpp

run:
	./driver

val:
	valgrind -s --leak-check=full ./driver

gdb:
	gdb ./driver

clean:
	rm *.o driver

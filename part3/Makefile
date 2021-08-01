#!make -f

CXX=clang++-9 
HEADERS=node.cpp
OBJECTS=main.o

run: demo
	./$^

demo: Demo.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o demo


clean:
	rm -f *.o demo test
	rm -f Test2.cpp

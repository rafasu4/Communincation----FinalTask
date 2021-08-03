#!make -f

CXX=clang++-9 

main.o:main.o 




run:select.h select.c
	./$^

main:  
	clang++ main.cpp -o node
	./node 


%.o: %.cpp
	$(CXX) $(CXXFLAGS) --compile $< -o $@

clean:
	rm -f *.o demo test

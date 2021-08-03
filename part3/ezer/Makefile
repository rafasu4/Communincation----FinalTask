all: sender measure

sender: sender.o 
	gcc -Wall -o sender sender.o

measure: measure.o 
	gcc -Wall -o measure measure.o

measure.o: measure.c 
	gcc -Wall -c measure.c

sender.o: sender.c 
	gcc -Wall -c sender.c

.PHONY: clean all 

clean:
	rm -f *.o sender measure
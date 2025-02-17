program: server.o
	g++ server.o -o program

server.o: server.cpp
	g++ -c server.cpp
	
clean:
	rm -f *o.program
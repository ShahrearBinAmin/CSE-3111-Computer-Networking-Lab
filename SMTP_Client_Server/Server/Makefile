CC=g++ -std=c++11
#will make executable file
server: server.o
	$(CC) server.o -o server
#this server.o will make compiled object file
server.o: server.cpp
	$(CC) -c server.cpp
#delete object file and executable file
clean:
	rm *.o server


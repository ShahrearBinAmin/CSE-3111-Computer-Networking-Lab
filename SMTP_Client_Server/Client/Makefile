CC=g++ -std=c++11
#will make executable file
client: client.o
	$(CC) client.o -o client
#this server.o will make compiled object file
client.o: client.cpp
	$(CC) -c client.cpp
#delete object file and executable file
clean:
	rm *.o client


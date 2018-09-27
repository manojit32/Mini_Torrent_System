all:	tracker1	tracker2	client

tracker1:	tracker1.cpp
	g++ tracker1.cpp -o tracker1 -lpthread

tracker2:	tracker2.cpp
	g++ tracker2.cpp -o tracker2 -lpthread

client:	client.cpp
	g++ client.cpp -o client -w -lm `libgcrypt-config --libs`

clean:	
	rm -rf *.o tracker1
	rm -rf *.o tracker2
	rm -rf *.o client



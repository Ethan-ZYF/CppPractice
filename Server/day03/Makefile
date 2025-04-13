server:
	g++ utils.cpp client.cpp -o client && \
	g++ utils.cpp server.cpp Epoll.cpp InetAddress.cpp Socket.cpp -o server
clean:
	rm server && rm client
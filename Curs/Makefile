ServerClient:
	gcc server.c DieWithError.c -o server -lpthread
	gcc client1.c DieWithError.c -o client1
	gcc client2.c DieWithError.c -o client2
	chmod +x start.sh
	./start.sh
clean:
	rm client1 client2 server

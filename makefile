all:
	gcc -o server serv_test.c -lpthread
	gcc -o client client_test.c -lpthread
server:
	gcc -o server serv_test.c -lpthread
client:
	gcc -o client client_test.c -lpthread
clean:
	rm server
	rm client
	

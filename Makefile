serveur :
	gcc -o serveur server/server.c -lpthread -Wall
client :
	gcc -o cli client/client.c -Wall
cleanServeur : 
	rm serveur
cleanClient :
	rm cli
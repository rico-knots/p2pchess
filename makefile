build: src/**
	gcc -o out/client src/client.c
	gcc -o out/server src/server.c
	echo "Finished"

run: build
	./out/client

hello:
	echo "Hello"

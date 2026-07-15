build: src/**
	gcc -o out/client src/networking/client.c
	gcc -o out/server src/networking/server.c
	echo "Finished"

run: build
	./out/server hello & ./out/client

hello:
	echo "Hello"

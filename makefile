build: src/**
	gcc -o out/client src/networking/client.c src/networking/protocol.c src/chess.c
	gcc -o out/server src/networking/server.c src/networking/protocol.c src/chess.c
	echo "Finished"

run: build
	./out/server hello & ./out/client

hello:
	echo "Hello"

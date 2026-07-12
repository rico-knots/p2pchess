build: src/**
	gcc -o out/client src/client.c
	gcc -o out/server src/server.c
	echo "Finished"

run: build
	./out/client

run-window: build-glad
	gcc src/window.c -lglfw -Lout/lib -lgl -o out/window
	./out/window

build-glad:
	gcc -c src/gl.c -o out/lib/gl.o
	ar rcs out/lib/libgl.a out/lib/gl.o

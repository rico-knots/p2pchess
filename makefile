build: src/**
	gcc -o out/client src/networking/client.c src/networking/protocol.c src/engine/chess.c
	gcc -o out/server src/networking/server.c src/networking/protocol.c src/engine/chess.c

run: build
	./out/server hello & ./out/client

host: build
	./out/client serv

run-window: build-glad
	gcc src/ui/window.c -lglfw -Lout/lib -lgl -lm -o out/ui/window
	cp -r src/shaders out/shaders
	cp -r src/assets out/assets
	./out/ui/window

build-glad:
	gcc -c src/gl.c -o out/lib/gl.o
	ar rcs out/lib/libgl.a out/lib/gl.o

format:
	find src -regex '.*\.\(c\|h\|cpp\|hpp\)' -exec clang-format -i {} +
	echo "Formatted!"
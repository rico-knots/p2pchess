CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -Isrc/include -Iout/lib
LDFLAGS := -Lout/lib -lgl -lglfw -lm

OUT := out
OBJ := $(OUT)/obj


# --- Source groups ---------------------------------------------------------
SRC_ENGINE := $(wildcard src/engine/*.c)
SRC_UI     := $(wildcard src/ui/*.c)
SRC_NET    := $(wildcard src/networking/*.c)
 
# client.c / server.c each have their own main() — keep them out of the
# shared game object list so the main chess app doesn't get two mains.
SRC_NET_SHARED := $(filter-out src/networking/client.c src/networking/server.c, $(SRC_NET))
 
# Everything the windowed chess app needs, besides main.c itself
SRC_GAME := $(SRC_ENGINE) $(SRC_UI) $(SRC_NET_SHARED)
OBJ_GAME := $(patsubst src/%.c, $(OBJ)/%.o, $(SRC_GAME))
OBJ_MAIN := $(OBJ)/main.o
 
# Client/server test binaries
OBJ_CLIENT := $(OBJ)/networking/client.o $(patsubst src/%.c, $(OBJ)/%.o, $(SRC_ENGINE) src/networking/protocol.c)
OBJ_SERVER := $(OBJ)/networking/server.o $(patsubst src/%.c, $(OBJ)/%.o, $(SRC_ENGINE) src/networking/protocol.c)
 
# --- Top-level targets -------------------------------------------------------
.PHONY: all run run-window host build-glad format clean
 
all: run-window
 
# Compile every .c into out/obj/<same path>.o, auto-tracking header deps
$(OBJ)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@
 
-include $(OBJ_GAME:.o=.d) $(OBJ_MAIN:.o=.d)
 
# --- glad static lib ---------------------------------------------------------
$(OUT)/lib/libgl.a: src/gl.c
	@mkdir -p $(OUT)/lib
	$(CC) -c src/gl.c -o $(OUT)/lib/gl.o
	ar rcs $@ $(OUT)/lib/gl.o
 
build-glad: $(OUT)/lib/libgl.a
 
# --- Windowed chess app -------------------------------------------------------
$(OUT)/chess: build-glad $(OBJ_GAME) $(OBJ_MAIN)
	@mkdir -p $(OUT)
	$(CC) $(OBJ_GAME) $(OBJ_MAIN) -o $@ $(LDFLAGS)
 
run-window: $(OUT)/chess
	@mkdir -p $(OUT)/shaders $(OUT)/assets
	cp -r src/shaders/. $(OUT)/shaders/
	cp -r src/assets/. $(OUT)/assets/
	./$(OUT)/chess
 
# --- Networking test client/server --------------------------------------------
$(OUT)/client: $(OBJ_CLIENT)
	@mkdir -p $(OUT)
	$(CC) $(OBJ_CLIENT) -o $@ $(LDFLAGS)
 
$(OUT)/server: $(OBJ_SERVER)
	@mkdir -p $(OUT)
	$(CC) $(OBJ_SERVER) -o $@ $(LDFLAGS)
 
run: $(OUT)/server $(OUT)/client
	./$(OUT)/server hello & ./$(OUT)/client
 
host: $(OUT)/client
	./$(OUT)/client serv
 
# --- Housekeeping -------------------------------------------------------------
format:
	find src -regex '.*\.\(c\|h\|cpp\|hpp\)' -exec clang-format -i {} +
	@echo "Formatted!"
 
clean:
	rm -rf $(OUT)
 

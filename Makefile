CC=gcc
CFLAGS=-O2 -Wall -Wextra
LDFLAGS=-lssl -lcrypto

SRC = dispatch.c exec.c main.c http.c hmac.c github.c json.c jsmn.c
OBJ = $(SRC:%.c=build/%.o)

BIN = out/ghd

all: $(BIN)

build:
	mkdir -p build

out:
	mkdir -p out

build/%.o: %.c | build
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): out $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)

clean:
	rm -rf build out

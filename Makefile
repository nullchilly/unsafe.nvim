.DEFAULT_GOAL = build

CFLAGS = -Wall -Werror -fPIC -Wl,-undefined -Wl,dynamic_lookup
SRC = $(wildcard src/*.c)
INCLUDES = $(wildcard src/*.h)

ifeq ($(OS),Windows_NT)
	MKD = -mkdir
	RM = cmd /C rmdir /Q /S
	CC = gcc
	TARGET := build\unsafe.dll
else
	MKD = mkdir -p
	RM = rm -rf
	TARGET := build/unsafe.so
endif

build: $(SRC) $(INCLUDES)
	$(MKD) build
	$(CC) -O3 $(CFLAGS) -shared $(SRC) -o $(TARGET)

debug: $(SRC) $(INCLUDES)
	$(MKD) build
	$(CC) -O0 -g -fsanitize=address $(CFLAGS) -shared $(SRC) -o $(TARGET)

clean:
	$(RM) build

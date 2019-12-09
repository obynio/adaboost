CC=gcc
LDLIBS= -lm -lSDL -lSDL_image -lpthread
CFLAGS= -Wall -Wextra -std=c99 -pedantic -O3
CFLAGS += $$(pkg-config --cflags gtk+-3.0)
LDFLAGS += $$(pkg-config --libs gtk+-3.0)
CFLAGS += $$(pkg-config --cflags json-c)
LDFLAGS += $$(pkg-config --libs json-c)

TARGET=htcpcp

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f *.o
	rm -f $(TARGET)

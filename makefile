CC := gcc
CFLAGS := -Wall -Werror
LIBS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Define the target executables
TARGETS := main
SRC_MAIN := main.c

all: $(TARGETS)

main: $(SRC_MAIN)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

main_rl: $(SRC_MAIN_RL)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean


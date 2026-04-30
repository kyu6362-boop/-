CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -O2
TARGET  = shadowverse
SRCS    = sv_main.c sv_game.c sv_cards.c

$(TARGET): $(SRCS) sv_defs.h
	$(CC) $(CFLAGS) -o $@ $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: clean

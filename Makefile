CC = clang
override CFLAGS += -g -Wno-everything
override CFLAGS += -std=gnu99

SRCS = $(shell find . -name '.ccls-cache' -type d -prune -o -type f -name '*.c' -print)
OBJS = $(patsubst %.c, %.o, $(SRCS))

main: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o patchable

clean:
	rm -f $(OBJS) main

CC = gcc
CFLAGS = -Wall -Wextra -Wno-implicit-fallthrough -std=gnu17 -fPIC -O2
LDFLAGS = -shared -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=reallocarray -Wl,--wrap=free -Wl,--wrap=strdup -Wl,--wrap=strndup

.PHONY: clean

tests.e: nand_example.c libnand.so
	gcc -L. -o $@ $< -lnand

libnand.so: nand.o memory_tests.o
	gcc $(LDFLAGS) -o $@ $^

clean:
	rm -rf *.o libnand.so tests.e
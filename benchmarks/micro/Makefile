CC ?= clang
CFLAGS ?= -O3 -Wall -Wextra
# -march=native for GCC/Linux; Apple Clang does not support it
ifeq ($(shell uname -s),Linux)
CFLAGS += -march=native
endif

BINS = icache_footprint dcache_stride pointer_chase branchy

all: $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(BINS)

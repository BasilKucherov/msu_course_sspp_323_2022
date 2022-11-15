CC = g++
LIBS = -pthread
CFLAGS = -Wall -Werror -O0 -std=gnu++11

PROGS = is_sorted thread_merge_sort array_generator

all: $(PROGS)

% : %.cpp
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

clean:
	rm -f $(PROGS)
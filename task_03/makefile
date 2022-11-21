CC = g++
LIBS = -lpapi
CFLAGS = -Wall -Werror -std=gnu++11

PROGS = papi_matrix_mul square_matrix_generator

all: $(PROGS)

% : %.cpp
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

clean:
	rm -f $(PROGS)


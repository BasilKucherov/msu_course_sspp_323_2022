all: main.cpp
	gcc main.cpp -o multiply_matrices -lstdc++ -Wall -Werror -fsanitize=address 
	gcc generate_square_matrix.cpp -o generate_square_matrix -lstdc++ -Wall -Werror -fsanitize=address 
 

test: 
	gcc main.cpp -o multiply_matrices -lstdc++ -Wall -Werror 
	gcc generate_square_matrix.cpp -o generate_square_matrix -lstdc++ -Wall -Werror 


clean:
	rm multiply_matrices
	rm generate_square_matrix

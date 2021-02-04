all:
	g++ main.c glad.c -o PDB -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
clean:
	rm PDB

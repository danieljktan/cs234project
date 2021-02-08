all:
	g++ main.c glad.c -Wno-unused-result -O2 -o PDB -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
clean:
	rm PDB

all:
	g++ main.c glad.c -o PDBViewer -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
clean:
	rm PDBViewer

all:
	g++ main.c glad.c -o program -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
clean:
	rm a.out

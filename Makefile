APP_NAME=ConvexHull
FLAGS=-Wall -Wunreachable-code -pedantic -fsanitize=address

c-version:
	gcc $(FLAGS) ConvexHull.c -lm -o $(APP_NAME)

cpp-version:
	g++ $(FLAGS) ConvexHull.cpp -o $(APP_NAME)

clean:
	rm -rf $(APP_NAME)
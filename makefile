game: main.cpp entity.cpp GameController.cpp
	clang++ main.cpp entity.cpp GameController.cpp -l splashkit -Wall -Wextra -o game
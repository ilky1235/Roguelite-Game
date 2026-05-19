CXX = clang++
CXXFLAGS = -Wall -Wextra 
LIBS = -l splashkit

SRCS = main.cpp entity.cpp player.cpp enemy.cpp projectile.cpp GameController.cpp
TARGET = game

$(TARGET): $(SRCS)
	$(CXX) $(SRCS) $(CXXFLAGS) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)
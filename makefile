CXX = clang++
CXXFLAGS = -Wall -Wextra 
LIBS = -l splashkit

SRCS = main.cpp entity.cpp player.cpp enemy.cpp projectile.cpp GameController.cpp room.cpp level_manager.cpp
TARGET = game

$(TARGET): $(SRCS)
	$(CXX) $(SRCS) $(CXXFLAGS) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)
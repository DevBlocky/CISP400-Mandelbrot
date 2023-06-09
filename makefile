SRC_DIR := ./src
OBJ_DIR := .
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lpthread -O3
CPPFLAGS := -g -Wall -fpermissive -std=c++17 -O3
CXXFLAGS :=
TARGET := mandelbrot

$(TARGET): $(OBJ_FILES)
	g++ -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

run:
	./$(TARGET)

clean:
	rm $(TARGET) *.o

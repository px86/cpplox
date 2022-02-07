CXX = g++
CXXFLAGS = -Wall -Werror -Wpedantic -std=c++17 -I$(INCLUDE_DIR)
LIBS =
INCLUDE_DIR = src/include
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)


lox: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp $(INCLUDE_DIR)/%.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf lox $(OBJ)

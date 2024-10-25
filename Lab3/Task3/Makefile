CXX = g++
CXXFLAGS = -Wall -std=c++11
LIBS = -framework OpenGL -lGLEW -lglfw -lm
SRC = Lab3_sphere.cpp
OUT = sphere

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC) $(LIBS) -I/opt/homebrew/include -L/opt/homebrew/lib

clean:
	rm -f $(OUT)

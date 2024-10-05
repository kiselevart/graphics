CXX = g++
CXXFLAGS = -Wall -std=c++11
LIBS = -lglfw -lGL -lGLEW

TARGET = opengl_triangle_indexed
SRCS = hello_triangle_indexed.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	rm -f $(TARGET)
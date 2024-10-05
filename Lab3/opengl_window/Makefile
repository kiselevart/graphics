CXX = g++
CXXFLAGS = -Wall -std=c++11
LIBS = -lglfw -lGL -lGLEW

TARGET = opengl_program
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	rm -f $(TARGET)

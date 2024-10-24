CXX = g++
CXXFLAGS = -Wall -std=c++11
LIBS = -lglfw -lGL -lGLEW -I/usr/include

TARGET = hello
SRCS = box.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

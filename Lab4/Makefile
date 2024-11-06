CXX = clang++
CXXFLAGS = -I/opt/homebrew/Cellar/glfw/3.4/include -std=c++17 -stdlib=libc++
LDFLAGS = -L/opt/homebrew/Cellar/glfw/3.4/lib -lglfw -lGL -framework OpenGL

# Source files 
SRCS = Task1/Lab4_textured_box.cpp /Users/kisel/graphics/dependencies/glad/src/glad.c

# Header files (correct paths for glad headers and other includes)
INCLUDES = -I/Users/kisel/graphics/dependencies/glad/include/ -I/usr/local/include -ITask1

# Output executable
TARGET = Lab4

# Compile the program
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) $(LDFLAGS) -o $(TARGET)

# Clean the build
clean:
	rm -f $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

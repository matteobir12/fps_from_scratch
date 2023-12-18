CXX = g++ 
CXXFLAGS = -Wall -Wextra -O2
LDLIBS = -lopengl32 -lglew32 -lfreeglut -lglu32

TARGET = build/main.exe
OBJDIR = build
SRCDIR = src
INCDIR = include

# Automatically list out the source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
# Replace the directory and extension of the source files to point to the object directory and change extension to .o
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

# Pattern rule for object files, which considers the include directory for header files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	del /Q $(OBJDIR)\\*

dev: all
	@echo Running $(TARGET)
	@$(TARGET)

.PHONY: all clean
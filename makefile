CXX = g++ 
CXXFLAGS = -Wall -Wextra 
LDLIBS = lib/libglfw3.a -lgdi32 -luser32 -lglew32 -lopengl32

TARGET = build/main.exe
OBJDIR = build
SRCDIR = src
INCDIR = include
LIBDIR = lib
BINDIR := bin

# Automatically list out the source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
# Replace the directory and extension of the source files to point to the object directory and change extension to .o
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -L$(LIBDIR) $(LDLIBS)

# Pattern rule for object files, which considers the include directory for header files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

build:
	cp $(BINDIR)/*.dll $(OBJDIR)/

clean:
	del /Q $(OBJDIR)\*.o
	del /Q $(OBJDIR)\main.exe
# del /Q $(OBJDIR)\*.dll

dev: CXXFLAGS += -g -O0
dev: all
	@$(TARGET)

run:
	@$(TARGET)

prod: CXXFLAGS += -O2
prod:	all

.PHONY: all clean
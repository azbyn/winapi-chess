#CXX := i686-w64-mingw32-clang++
#CXX := x86_64-w64-mingw32-clang++
GCC := x86_64-w64-mingw32-g++
CLANG := x86_64-w64-mingw32-clang++
CXX := $(GCC)
LD :=  $(GCC)
#SRC_DIR := .
BUILD_DIR := build
TARGET := ../a.exe

SRCS := $(shell find . -name "*.cpp" -printf '%P\n')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
INCL := $(wildcard *.h)
#THIS_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

BOTH_FLAGS := -g -mwindows -municode -mconsole
#-mwin32

#CPPFLAGS := $(BOTH_FLAGS) $(INC_FLAGS)
CPPFLAGS := $(BOTH_FLAGS) $(INC_FLAGS)
CPPFLAGS += -Wall -Wextra -std=c++17 -pedantic -Og
#-ferror-limit=10
#-Wno-unused-const-variable \
#-DOHMD_STATIC
#



#LDFLAGS := -static-libstdc++ -static -pthread
LDFLAGS := $(BOTH_FLAGS)
LDFLAGS += -static-libstdc++ -static-libgcc -static -pthread -lwinmm
#-ldbghelp

#this must be the last
#LDFLAGS += -limagehlp

.PHONY: build
build: $(TARGET)

.PHONY: rebuild
rebuild: clean build

.PHONY: compile
compile: clean $(OBJS)

$(TARGET): $(OBJS)
	$(LD) -o $@ $(OBJS) $(LDFLAGS)

#$(BUILD_DIR)/%.cpp.o: %.cpp ${INCL}

$(BUILD_DIR)/%.cpp.o: %.cpp
	@$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) -c -o $@ $<


.PHONY: clean
#don't clean stockfish, it's large and i'm not gonna change that
clean:
	@$(RM) $(BUILD_DIR)/Chess/*.o $(BUILD_DIR)/Chess/chess/*.o $(BUILD_DIR)/Chess/core/*.o $(TARGET)

#-include $(DEPS)

MKDIR_P ?= mkdir -p

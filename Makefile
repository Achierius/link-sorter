#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need SDL3 (http://www.libsdl.org) which is still unreleased/unpackaged.

#CXX = g++
#CXX = clang++

BUILD_DIR = build

# the actual application
CORE_SOURCES = main.cpp home_window.cpp web_view.cpp
OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(CORE_SOURCES)))))

# external/imgui
IMGUI_DIR = external/imgui
IMGUI_BUILD_DIR = $(BUILD_DIR)/$(IMGUI_DIR)
IMGUI_SOURCES_RAW = imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp
IMGUI_SOURCES_RAW += backends/imgui_impl_sdl3.cpp backends/imgui_impl_sdlrenderer3.cpp
IMGUI_SOURCES = $(addprefix $(IMGUI_DIR)/, $(IMGUI_SOURCES_RAW))
IMGUI_OBJS = $(addprefix $(IMGUI_BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(IMGUI_SOURCES)))))
OBJS += $(IMGUI_OBJS)

# external/stb
STB_DIR = external/stb
STB_BUILD_DIR = $(BUILD_DIR)/$(STB_DIR)
STB_SOURCES = $(STB_DIR)/stb_image.cpp
STB_OBJS = $(addprefix $(STB_BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(STB_SOURCES)))))
OBJS += $(STB_OBJS)

EXE = $(BUILD_DIR)/application

UNAME_S := $(shell uname -s)
LINUX_GL_LIBS = -lGL

CXXFLAGS = -std=c++11 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I $(STB_DIR)
CXXFLAGS += -g -Wall -Wformat
LIBS =

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += -ldl $(shell pkg-config sdl3 webkitgtk-6.0 gtk4 --libs)

	CXXFLAGS += -Isrc/ $(shell pkg-config sdl3 webkitgtk-6.0 gtk4 --cflags)
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo $(shell sdl3-config --libs)
	LIBS += -L/usr/local/lib -L/opt/local/lib

	CXXFLAGS += $(shell pkg-config sdl3 --cflags)
	CXXFLAGS += -I/usr/local/include -I/opt/local/include
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(OS), Windows_NT)
    ECHO_MESSAGE = "MinGW"
    LIBS += -lgdi32 -lopengl32 -limm32 $(shell pkg-config --static --libs sdl3)

    CXXFLAGS += $(shell pkg-config --cflags sdl3)
    CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

$(BUILD_DIR)/%.o:src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# STB
$(STB_BUILD_DIR)/%.o:$(STB_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# IMGUI
$(IMGUI_BUILD_DIR)/%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(IMGUI_BUILD_DIR)/%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)

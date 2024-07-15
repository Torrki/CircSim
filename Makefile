GTK4_FOUND = $(shell pkg-config --exists gtk4; echo $$?)

ifeq ($(GTK4_FOUND), 1)
$(error GTK4 not found)
endif

$(info GTK4 package found)
CFLAGS_GTK4 = $(shell pkg-config --cflags gtk4)
LIBS_GTK4 = $(shell pkg-config --libs gtk4)
SRCS = $(shell find | grep .cpp)
BIN = CircSim

all:
	g++ -I. -I./CircObjects $(CFLAGS_GTK4) $(SRCS) -o $(BIN) $(LIBS_GTK4)
	

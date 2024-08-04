GTK4_FOUND = $(shell pkg-config --exists gtk4; echo $$?)
BASECOMP_FOUND = $(shell find Componenti/libbasecomp.so; echo $$?)

ifeq ($(GTK4_FOUND), 1)
$(error GTK4 not found)
endif

ifeq ($(BASECOMP_FOUND), 1)
$(error BASECOMP not found)
endif

$(info GTK4 package found)
$(info BASECOMP library found)
BIN = CircSim
CFLAGS_GTK4 = $(shell pkg-config --cflags gtk4)
LIBS_GTK4 = $(shell pkg-config --libs gtk4)
SRCS = $(shell find | grep .cpp | grep -v Componenti)
MY_DIRS = -I. -I./CircObjects -I./Interfaces -I./Componenti -I./Graph

all:
	g++ $(SRCS) -o $(BIN) $(CFLAGS_GTK4) $(MY_DIRS) -LComponenti $(LIBS_GTK4) -lbasecomp
	

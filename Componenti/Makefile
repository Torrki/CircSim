SOURCES=$(shell find | grep .cpp)
MY_DIRS = -I. -I../CircObjects -I../Interfaces

compile:
	g++ -fPIC -shared $(SOURCES) $(MY_DIRS) -o libbasecomp.so
	$(INFO libbasecomp.so compilato)

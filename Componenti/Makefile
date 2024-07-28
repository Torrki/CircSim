SOURCES=$(shell find | grep .cpp)
MY_DIRS = -IResistore -ICondensatore -IInduttore -I../CircObjects -I../drawable

compile:
	g++ -fPIC -shared $(SOURCES) $(MY_DIRS) -o libbasecomp.so

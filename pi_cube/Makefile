CC=g++ -Wall
DEFINES=
CFLAGS=-O3 -std=c++14 
INCLUDES=-I/usr/local/include/eigen -I/usr/local/include/Box2D
LIBS=-L/usr/local/lib -lrt -lliquidfun -lpthread

CUBE=led_cube
CUBE_SOURCES=images/read_text_file.cpp imu/bno080.cpp cube_thread.cpp gpio/gpio.cpp main_thread.cpp liquid_fun/lf_animation.cpp
CUBE_OBJECTS=$(CUBE_SOURCES:.cpp=.o)

.PHONY: install
all: cube

cube: $(CUBE_SOURCES) $(CUBE)

$(CUBE): $(CUBE_OBJECTS)
		$(CC) $(CFLAGS) $(DEFINES) $(CUBE_OBJECTS) -o $@ $(LIBS)

.cpp.o:
		$(CC) $(CFLAGS) $(INCLUDES) -c $(DEFINES) $< -o $@

clean:
		rm -f *.o; rm -f $(CUBE)
	
install: led_cube
	cp $< /usr/local/bin/.

# Build AMB demo application

CC      = g++

CFLAGS  = -std=c++11 \
	  -Wall

LDFLAGS = -lpthread \
	  -lpulse \
	  -lpulse-simple

OBJS    = amb_buffers.o \
  	  amb_capture.o \
  	  amb_color.o \
  	  amb_demo.o \
  	  amb_grove.o \
	  amb_main.o \
	  amb_playback.o \
	  amb_ui.o

all: amb_demo

amb_demo: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cc
	$(CC) -c $(CFLAGS) $< -o $@

amb_buffers.o: amb_buffers.h
amb_capture.o: amb_capture.h amb_buffers.h
amb_color.o: amb_color.h amb_grove.h
amb_demo.o: amb_demo.h amb_buffers.h amb_capture.h amb_playback.h
amb_grove.o: amb_grove.h
amb_main.o: amb_demo.h amb_ui.h
amb_playback.o: amb_playback.h amb_buffers.h
amb_ui.o: amb_ui.h

.PHONY: clean

clean:
	rm -vf $(OBJS) amb_demo

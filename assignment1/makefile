# This is the makefile for ECE1387 Assignment 1 Maze Router
# Developer: 	Louis Chen (1000303502)
# Date:		Oct 7, 2018

CC := g++
RM := rm -f

CFLAGS := -Wall
LDFLAGS :=
LDLIBS := -lX11

OBJFILES := main.o Router.o GoalPath.o Track.o Display.o graphics.o
TARGET := MazeRouter-LouisChen

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) $(LDLIBS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	$(RM) $(OBJFILES) $(TARGET)


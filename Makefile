
all:
	gcc src/*.c -o qedit -g -Wall

debug:
	gcc src/*.c -o qedit -g -Wall
	.\qedit test
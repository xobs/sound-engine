all: sound

sound: sound.c wave-table.h
	gcc sound.c -o sound -Wall -g
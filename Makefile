ifeq ($(OS),Windows_NT)
    CFLAGS += -fuse-ld=lld -Z7 -MTd -DDESKTOP
	CC = clang-cl.exe
	OUTPUT = .\sound.exe
else
	CFLAGS += -o sound -Wall -g
	CC ?= gcc
	OUTPUT = sound
endif

all: $(OUTPUT)
	powershell -NoProfile -Command 'echo n | cmd /c "$(OUTPUT) | play -b 8 -c 1 -t u8 -r 7808 -"'

$(OUTPUT): sound.c wave-table.h note-table.h nyan.h
	$(CC) sound.c $(CFLAGS)
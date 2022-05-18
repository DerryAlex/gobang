CC = gcc
CFLAGS = -Wall -Wextra -Ofast -march=native -DNDEBUG
CFLAGS_DEBUG = -Wall -Wextra -Og -g
CFLAGS_GTK = `pkg-config --cflags gtk4`
LDFLAGS = 
LDFLAGS_GTK = `pkg-config --libs gtk4`

.PHONY: clean all

all:
	make -C ./ai libgobang_ai.a
	make gobang
	make gobang_gui

gobang: main.o referee.o ui/tui.o ai/libgobang_ai.a
	$(CC) main.o referee.o ui/tui.o ai/libgobang_ai.a -o gobang $(LDFLAGS)

gobang_gui: main.o referee.o ui/gui.o ai/libgobang_ai.a
	$(CC) main.o referee.o ui/gui.o ai/libgobang_ai.a -o gobang_gui $(LDFLAGS) $(LDFLAGS_GTK)
	
clean: 
	rm *.o
	rm ui/*.o

ui/tui.o: ui/tui.c ui/ui.h gobang.h
	$(CC) ui/tui.c -c -o ui/tui.o $(CFLAGS)

ui/gui.o: ui/gui.c ui/ui.h gobang.h
	$(CC) ui/gui.c -c -o ui/gui.o $(CFLAGS_GTK) $(CFLAGS)

referee.o: referee.c referee.h gobang.h
	$(CC) referee.c -c -o referee.o $(CFLAGS)

main.o: main.c gobang.h referee.h ui/ui.h ai/ai.h
	$(CC) main.c -c -o main.o $(CFLAGS)

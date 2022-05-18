CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -Ofast -march=native -DNDEBUG
CFLAGS_DEBUG = -Wall -Wextra -Og -g
LDFLAGS = 

.PHONY: clean

libgobang_ai.a: ai.o pattern.o map.o vcf.o timer.o search.o tran_table.o
	$(AR) rcs libgobang_ai.a ai.o pattern.o map.o vcf.o timer.o search.o tran_table.o

clean:
	rm *.o

ai.o: ai.c ai.h pattern.h map.h vcf.h timer.h search.h tran_table.h ../gobang.h
	$(CC) ai.c -c -o ai.o $(CFLAGS)

pattern.o: pattern.c pattern.h ../gobang.h
	$(CC) pattern.c -c -o pattern.o $(CFLAGS)

map.o: map.c map.h pattern.h tran_table.h ../gobang.h
	$(CC) map.c -c -o map.o $(CFLAGS)

vcf.o: vcf.c vcf.h timer.h map.h pattern.h ../gobang.h
	$(CC) vcf.c -c -o vcf.o $(CFLAGS)

timer.o: timer.c timer.h
	$(CC) timer.c -c -o timer.o $(CFLAGS)

search.o: search.c search.h timer.h tran_table.h ../gobang.h
	$(CC) search.c -c -o search.o $(CFLAGS)

tran_table.o: tran_table.c tran_table.h ../gobang.h
	$(CC) tran_table.c -c -o tran_table.o $(CFLAGS)


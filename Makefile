CC = gcc
CFLAGS = -std=c11 -Wall -Werror -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -g -O0
LDFLAGS = 

DESTDIR ?= 
PREFIX ?= /usr

TARGET0 = js2bas
OBJECT0 = $(SOURCE0:%.c=%.c.o)
SOURCE0 = main.c token.c parse.c

all: $(TARGET0)

clean:
	rm -f *.o

distclean: clean
	rm -f $(TARGET0) test.bas

dist: distclean
	tar cvf ../$(DIRNAME)-latest.txz ../$(DIRNAME)

install: $(TARGET0)
	cp $(TARGET0) $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(TARGET0)

$(TARGET0): $(OBJECT0)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.c.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<


CC=g++
sourcefolder=CoreTempMonitor/
PROGNAME=coretempmon
CFLAGS=-std=c++11 -Wall
coretempmon: $(sourcefolder)main.cpp
	$(CC) $(sourcefolder)main.cpp $(CFLAGS) -o $(PROGNAME)

prefix=/usr/local
    
install: coretempmon
	install -m 0755 $(PROGNAME) $(prefix)/bin

.PHONY: install

.PHONY: clean
clean:
	rm $(PROGNAME)

.PHONY: uninstall
uninstall:
	rm $(prefix)/bin/$(PROGNAME)

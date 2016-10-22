CC=g++
sourcefolder=CoreTempMonitor/
PROGNAME=coretempmon
coretempmon: main.cpp
	$(CC) $(sourcefolder)main.cpp -o $(PROGNAME)

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

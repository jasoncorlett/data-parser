.PHONY: run clean gen scratch

CFLAGS = -g -Wall -Wextra --pedantic
JSON = "Hello, World!"
OBJECTS = json.o parser.o enum.o
EXE = echo

build: clean gen $(EXE)
	rm -f $(OBJECTS)

$(EXE): $(OBJECTS)

gen:
	perl bin/gen-enum.pl

clean:
	rm -f $(OBJECTS) $(EXE)

run: build
	./$(EXE) '$(JSON)'

debug: clean build
	valgrind --leak-check=full --track-origins=yes ./$(EXE) '$(JSON)'

test: build
	prove -v

scratch:
	@gcc $(CFLAGS) scratch.c -o scratch
	@./scratch
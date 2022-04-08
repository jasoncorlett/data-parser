.PHONY: run clean debug scratch

CFLAGS = -g -Wall -Wextra --pedantic
JSON = "Hello, World!"
OBJECTS = json.o util.o node.o
EXE = echo

build: clean $(EXE)
	rm -f $(OBJECTS)

$(EXE): $(OBJECTS)

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
EXECUTABLE:=gofish
SOURCES=$(EXECUTABLE).c player.c card.c deck.c
OBJECTS=$(SOURCES:.c=.o)
CFLAGS= -Werror -Wall -std=gnu11 -Wno-missing-declarations -Wshadow

all: $(EXECUTABLE)

$(EXECUTABLE):$(OBJECTS)
	@echo EXE: the rulename is $@ and the first dependency is $<
	gcc $(CFLAGS) -o $@ $(OBJECTS)

$(OBJECTS):%.o:%.c
	@echo OBJ: the rulename is $@ and the first dependency is $<
	gcc $(CFLAGS) -c $<

debug:CFLAGS += -g
debug:$(OBJECTS)
	@echo EXE: the rulename is $@ and the first dependency is $<
	gcc $(CFLAGS) -o $@ $(OBJECTS) -dGO_DEBUG

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)



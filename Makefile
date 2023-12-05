# Returns all c files nested or not in $(1)
define collect_sources
	$(shell find $(1) -name '*.c')
endef

SOURCES = $(call collect_sources, src)
OBJECTS = $(patsubst %.c, objects/%.o, $(SOURCES))

LD_FLAGS = `pkg-config --libs sdl2 SDL2_image SDL2_mixer SDL2_ttf` -lm

.PHONY: build
all: build

build: $(OBJECTS)
	@echo "{Makefile} Creating the executable"
	@$(CC) $(OBJECTS) -o bin $(LD_FLAGS)

	@./bin

objects/%.o: %.c
	@# Making sure that the directory already exists before creating the object
	@mkdir -p $(dir $@)

	@echo "{Makefile} Building $@"
	@$(CC) -c $< -o $@


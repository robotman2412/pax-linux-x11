
# Options
CC            ?=/usr/bin/gcc
BUILD_DIR     ?=build
CCOPTIONS     ?=-c -Isrc -Icomponents/pax-gfx/src
LDOPTIONS     ?=
LIBS          ?=-lX11 -lm

# Sources
SOURCES        =$(shell find src -type f -name '*.c')
HEADERS        =$(shell find src -type f -name '*.h')

# Outputs
OBJECTS        =$(shell echo $(SOURCES) | sed -e 's/src/$(BUILD_DIR)/g;s/\.c/.c.o/g')
OBJECTS_DEBUG  =$(shell echo $(SOURCES) | sed -e 's/src/$(BUILD_DIR)/g;s/\.c/.c.debug.o/g')
OUT_PATH      ?=paxlinux

# Actions
.PHONY: all debug clean run components/pax-gfx/build/libpax.so

all: build/app.o
	@mkdir -p build
	@cp build/app.o $(OUT_PATH)

debug: build/app.debug.o
	@mkdir -p build
	@cp build/app.debug.o $(OUT_PATH)

clean:
	$(MAKE) -C components/pax-gfx -f Standalone.mk clean
	rm -rf build
	rm -f $(OUT_PATH)

run: $(OUT_PATH)
	./$(OUT_PATH)

# Regular files
build/app.o: $(OBJECTS) components/pax-gfx/build/libpax.so
	@mkdir -p $(shell dirname $@)
	@$(MAKE) --no-print-directory -C components/pax-gfx -f Standalone.mk all
	$(CC) $(LDOPTIONS) -o $@ $^ $(LIBS)

build/%.o: src/% $(HEADERS)
	@mkdir -p $(shell dirname $@)
	$(CC) $(CCOPTIONS) -o $@ $< $(LIBS)

# Debug files
build/app.debug.o: $(OBJECTS_DEBUG) components/pax-gfx/build/libpax.so
	@mkdir -p $(shell dirname $@)
	@$(MAKE) --no-print-directory -C components/pax-gfx -f Standalone.mk debug
	$(CC) $(LDOPTIONS) -o $@ $^ $(LIBS)

build/%.debug.o: src/% $(HEADERS)
	@mkdir -p $(shell dirname $@)
	$(CC) $(CCOPTIONS) -ggdb -o $@ $< $(LIBS)

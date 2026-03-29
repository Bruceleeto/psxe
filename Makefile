.ONESHELL:

CFLAGS := -m32 -g -DLOG_USE_COLOR `sdl2-config --cflags --libs`
CFLAGS += -O2 -march=native -fomit-frame-pointer -Wno-overflow -Wall -pedantic -Wno-address-of-packed-member -flto -lm

PLATFORM := $(shell uname -s)

ifeq ($(PLATFORM),Darwin)
	CFLAGS += -mmacosx-version-min=10.9 -Wno-newline-eof
endif

VERSION_TAG := $(shell git describe --always --tags --abbrev=0)
COMMIT_HASH := $(shell git rev-parse --short HEAD)
OS_INFO := $(shell uname -rmo)

SOURCES := $(wildcard psx/*.c)
SOURCES += $(wildcard psx/dev/*.c)
SOURCES += $(wildcard psx/dev/cdrom/*.c)
SOURCES += $(wildcard psx/input/*.c)
SOURCES += $(wildcard psx/disc/*.c)
SOURCES += $(wildcard frontend/*.c)

psxe: $(SOURCES)
	mkdir -p assets

	gcc $(SOURCES) -o psxe \
		-I"." \
		-I"psx" \
		-DOS_INFO="$(OS_INFO)" \
		-DREP_VERSION="$(VERSION_TAG)" \
		-DREP_COMMIT_HASH="$(COMMIT_HASH)" \
		$(CFLAGS)

clean:
	rm -f psxe

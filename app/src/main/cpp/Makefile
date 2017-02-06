NANOVG_INCLUDE=../../../nanovg/src

LDFLAGS = -lm -lGL -lglfw -ljack -lsoundpipe -lsndfile
CFLAGS = -g -Ih -I$(NANOVG_INCLUDE) -Wall 

# nanovg is riddled with these, and I can't seem to shake them
CFLAGS += -Wno-implicit-function-declaration

HFILES = h/orb.h h/synth.h

SRC = main.c run.c jack.c audio.c synth.c vals.c grid.c

OBJ = $(addprefix src/,$(SRC:.c=.o))

OBJ += $(NANOVG_INCLUDE)/nanovg.o 

default: orb 

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

src/vals.o: src/synth.o

orb: $(OBJ) $(HFILES)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

src/synth.c: spgen/ping.lua
	lua $<

h/synth.h: src/synth.c

install:
	cd ../../../../; ./gradlew installDebug
	adb shell am start -n com.paulbatchelor.orb/.MainActivity

clean:
	rm -rf orb $(OBJ)

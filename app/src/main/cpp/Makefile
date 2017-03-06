NANOVG_INCLUDE=../../../nanovg/src

LDFLAGS = -lm -lGL -lglfw -ljack -lsoundpipe -lsndfile
CFLAGS = -g -Ih -I$(NANOVG_INCLUDE) -Wall 

# nanovg is riddled with these, and I can't seem to shake them
CFLAGS += -Wno-implicit-function-declaration

HFILES = h/orb.h h/synth.h

SRC = main.c run.c jack.c audio.c synth.c vals.c grid.c motion.c cstack.c \
	  object.c avatar.c fsm.c orb_fsm.c modal.c revscm.c levels.c color.c

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

fsm_test: src/fsm_test.c src/fsm.o
	$(CC) $(CFLAGS) $< -o $@ src/fsm.o 

monitor: 
	adb logcat -s "orb" orb:I

clean:
	rm -rf orb $(OBJ)
	rm -rf fsm_test

cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -O3 -DNO_LIBSNDFILE -DBUILD_ANDROID")

include_directories(
    soundpipe/lib/faust/ 
    soundpipe/lib/fft/
    soundpipe/lib/kissfft/
    soundpipe/lib/inih
    soundpipe/h
    src/main/cpp/
    src/main/cpp/h
    nanovg/src
    )


add_library(native-audio-jni SHARED
    src/main/cpp/native-audio-jni.c 
    src/main/cpp/src/synth.c 
    src/main/cpp/h/synth.h 
    src/main/cpp/h/orb.h 
    src/main/cpp/src/audio.c 
    src/main/cpp/src/run.c
    src/main/cpp/src/vals.c
)

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             src/main/cpp/native-lib.cpp )

add_library(
    soundpipe SHARED
    soundpipe/lib/faust/CUI.h
    soundpipe/lib/kissfft/kiss_fftr.h
    soundpipe/lib/kissfft/kiss_fft.h
    soundpipe/lib/kissfft/kiss_fft.c
    soundpipe/lib/kissfft/kiss_fftr.c
    soundpipe/lib/inih/ini.h
    soundpipe/lib/inih/ini.c
    soundpipe/lib/fft/fft.c
    soundpipe/h/soundpipe.h
    soundpipe/modules/adsr.c
    soundpipe/modules/allpass.c
    soundpipe/modules/atone.c
    soundpipe/modules/autowah.c
    soundpipe/modules/bal.c
    soundpipe/modules/bar.c
    soundpipe/modules/base.c
    soundpipe/modules/biquad.c
    soundpipe/modules/biscale.c
    soundpipe/modules/bitcrush.c
    soundpipe/modules/blsaw.c
    soundpipe/modules/blsquare.c
    soundpipe/modules/bltriangle.c
    soundpipe/modules/butbp.c
    soundpipe/modules/butbr.c
    soundpipe/modules/buthp.c
    soundpipe/modules/butlp.c
    soundpipe/modules/clip.c
    soundpipe/modules/clock.c
    soundpipe/modules/comb.c
    soundpipe/modules/compressor.c
    soundpipe/modules/conv.c
    soundpipe/modules/count.c
    soundpipe/modules/crossfade.c
    soundpipe/modules/dcblock.c
    soundpipe/modules/delay.c
    soundpipe/modules/diode.c
    soundpipe/modules/dist.c
    soundpipe/modules/dmetro.c
    soundpipe/modules/drip.c
    soundpipe/modules/dtrig.c
    soundpipe/modules/dust.c
    soundpipe/modules/eqfil.c
    soundpipe/modules/expon.c
    soundpipe/modules/fftwrapper.c
    soundpipe/modules/fof.c
    soundpipe/modules/fofilt.c
    soundpipe/modules/fog.c
    soundpipe/modules/fold.c
    soundpipe/modules/foo.c
    soundpipe/modules/fosc.c
    soundpipe/modules/ftbl.c
    soundpipe/modules/gbuzz.c
    soundpipe/modules/hilbert.c
    soundpipe/modules/in.c
    soundpipe/modules/incr.c
    soundpipe/modules/jcrev.c
    soundpipe/modules/jitter.c
    soundpipe/modules/line.c
    soundpipe/modules/lpf18.c
    soundpipe/modules/maygate.c
    soundpipe/modules/metro.c
    soundpipe/modules/mincer.c
    soundpipe/modules/mode.c
    soundpipe/modules/moogladder.c
    soundpipe/modules/noise.c
    soundpipe/modules/nsmp.c
    soundpipe/modules/osc.c
    soundpipe/modules/oscmorph.c
    soundpipe/modules/padsynth.c
    soundpipe/modules/pan2.c
    soundpipe/modules/panst.c
    soundpipe/modules/pareq.c
    soundpipe/modules/paulstretch.c
    soundpipe/modules/pdhalf.c
    soundpipe/modules/peaklim.c
    soundpipe/modules/phaser.c
    soundpipe/modules/phasor.c
    soundpipe/modules/pinknoise.c
    soundpipe/modules/pitchamdf.c
    soundpipe/modules/pluck.c
    soundpipe/modules/port.c
    soundpipe/modules/posc3.c
    soundpipe/modules/progress.c
    soundpipe/modules/prop.c
    soundpipe/modules/pshift.c
    soundpipe/modules/ptrack.c
    soundpipe/modules/randh.c
    soundpipe/modules/randi.c
    soundpipe/modules/randmt.c
    soundpipe/modules/random.c
    soundpipe/modules/reson.c
    soundpipe/modules/reverse.c
    soundpipe/modules/revsc.c
    soundpipe/modules/rms.c
    soundpipe/modules/rpt.c
    soundpipe/modules/samphold.c
    soundpipe/modules/saturator.c
    soundpipe/modules/scale.c
    soundpipe/modules/scrambler.c
    soundpipe/modules/sdelay.c
    soundpipe/modules/slice.c
    soundpipe/modules/smoothdelay.c
    soundpipe/modules/streson.c
    soundpipe/modules/switch.c
    soundpipe/modules/tabread.c
    soundpipe/modules/tadsr.c
    soundpipe/modules/tblrec.c
    soundpipe/modules/tbvcf.c
    soundpipe/modules/tdiv.c
    soundpipe/modules/tenv.c
    soundpipe/modules/tenv2.c
    soundpipe/modules/tenvx.c
    soundpipe/modules/tevent.c
    soundpipe/modules/tgate.c
    soundpipe/modules/thresh.c
    soundpipe/modules/timer.c
    soundpipe/modules/tin.c
    soundpipe/modules/tone.c
    soundpipe/modules/trand.c
    soundpipe/modules/tseg.c
    soundpipe/modules/tseq.c
    soundpipe/modules/vdelay.c
    soundpipe/modules/vocoder.c
    soundpipe/modules/waveset.c
    soundpipe/modules/wpkorg35.c
    soundpipe/modules/zitarev.c
    )

add_library(nanovg SHARED
    nanovg/src/nanovg.c
)

find_library( # Sets the name of the path variable.
    log-lib
    GLESv2
    log )

target_link_libraries( native-audio-jni
                       soundpipe 
                       OpenSLES
                       android
                       nanovg
                       log)

target_link_libraries(nanovg
    GLESv2
    EGL
    log
    )

target_link_libraries( native-lib
                       android
                       nanovg
                        
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} 
                       )

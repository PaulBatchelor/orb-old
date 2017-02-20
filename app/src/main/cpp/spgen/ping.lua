require("spgen.sp")

SP:nchan(1)

synth = {}
synth.sine = SP:ftbl("sine", 2048)
synth.gs = SP:gen_sine(synth.sine)
synth.tick = VAR:new("gate", 0.0)
synth.env = SP:tenvx("env", 0.005, 0.0051, 0.02)
--synth.fosc = SP:fosc("fm", synth.sine, 440, 0.05, 1, 1, 1)
synth.randh = SP:randh("randh", -0.02, 0.02, 1000)
synth.thresh = SP:thresh("thresh", 0.5, 2)
synth.rev = SP:revsc("rev", 0.97, 10000)
synth.trand = SP:trand("tr", 1900, 10000)
synth.delay = SP:delay("delay", 0.9, 1.1)
synth.butlp = SP:butlp("lpf", 2000)
synth.modal = SP:modal("mode")

synth.voice1 = SP:fosc("voice1", synth.sine, 440, 0.02, 1, 1, 1)
synth.voice2 = SP:fosc("voice2", synth.sine, 440, 0.02, 1, 1, 1.5)

synth.port1 = SP:port("port1", 0.06)
synth.port2 = SP:port("port2", 0.08)
synth.freq1 = VAR:new("freq1", 440)
synth.freq2 = VAR:new("freq2", 330)

--synth.vrand1 = SP:randi("vrand1", 0, 0.1, 0.2)
--synth.vrand2 = SP:randi("vrand2", 0, 0.1, 0.2)

--synth.met1 = SP:dmetro("met1", 4)
--synth.met2 = SP:dmetro("met2", 6)
--
--synth.mt1 = SP:maytrig("mt1", 0.7)
--synth.tenv1= SP:tenv("tenv1", 3, 1, 4)
--
--synth.mt2 = SP:maytrig("mt2", 0.8)
--synth.tenv2= SP:tenv("tenv2", 4, 1, 4)

--synth.voice3 = SP:fosc("voice3", synth.sine, 440, 0.01, 1, 1, 1)
--synth.voice4 = SP:fosc("voice4", synth.sine, 440, 0.01, 1, 1, 1)

synth.lfo1 = SP:osc("lfo1", synth.sine, 0.1, 1, 0)
synth.lfo2 = SP:osc("lfo2", synth.sine, 0.14, 1, 0.2)

function compute_voices(tmp1, tmp2, tmp3, met, osc)
    --synth.vrand1:compute({-1, tmp1})
    --synth.voice1:set("amp", tmp1)

    synth.port1:compute({synth.freq1, tmp3})
    synth.voice1:set("freq", tmp3)
    synth.voice1:compute({-1, osc})

    synth.lfo1:compute({-1, tmp3})
    SP:var_biscale(tmp3, 0, 1)
    SP:mul(osc, osc, tmp3)

    --synth.met1:compute({-1, met})
    --synth.mt1:compute({met, tmp2})
    --synth.tenv1:compute({tmp2, tmp3})
    --SP:mul(osc, osc, tmp3)

    --synth.vrand2:compute({-1, tmp1})
    --synth.voice2:set("amp", tmp1)
    
    synth.port2:compute({synth.freq2, tmp3})
    synth.voice2:set("freq", tmp3)
    synth.voice2:compute({-1, tmp1})

    synth.lfo2:compute({-1, tmp3})
    SP:var_biscale(tmp3, 0, 1)
    SP:mul(tmp1, tmp1, tmp3)

    --synth.met2:compute({-1, met})
    --synth.mt2:compute({met, tmp2})
    --synth.tenv2:compute({tmp2, tmp3})
    --SP:mul(tmp1, tmp1, tmp3)
    SP:add(osc, osc, tmp1)

    
    --synth.voice3:compute({-1, tmp})
    --SP:add(osc, osc, tmp)
    --synth.voice4:compute({-1, tmp})
    --SP:add(osc, osc, tmp)
end

h = io.open("h/synth.h", "w")
c = io.open("src/synth.c", "w")

SP:set_header(h)
SP:set_source(c)

SP:print_header(synth)

SP:print_synth(synth,
function (syn)
    env = 0
    tick = 1
    tmp = 2
    fm = 3
    rev = 4
    out = 5
    tenv = 6

    -- use out since it's not being used yet
    synth.thresh:compute({synth.tick, tick})
    synth.trand:compute({tick, out})
    synth.randh:set("freq", out)
    synth.randh:compute({-1, fm})
    synth.env:compute({tick, env})
    SP:mul(out, env, fm)
    -- Compute delay to tmp
    synth.delay:compute({out, tmp})
    -- Filter delay to rev (not being used yet)
    synth.butlp:compute({tmp, rev})
    SP:var_scale(rev, 0.6)
    -- add filtered delay back into dry signal
    SP:add(out, out, rev)
    -- Modal collision to rev (not being used yet)
    synth.modal:compute({-1, rev})

    -- add collision back into dry signal
    SP:add(out, out, rev)

    -- compute 4 fm oscilators, discard for now
    -- rev tick not used, so use those 
    compute_voices(tick, fm, env, tenv, rev)
    SP:add(out, out, rev)

    synth.rev:compute({out, out, rev, tmp})
    SP:var_scale(rev, 0.05)
    SP:add(out, out, rev)
    SP:output(out)
end, 10)

h:close()
c:close()

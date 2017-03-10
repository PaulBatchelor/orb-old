require("spgen.sp")

SP:nchan(1)

synth = {}
synth.sine = SP:ftbl("sine", 2048)
synth.gs = SP:gen_sine(synth.sine)
synth.tick = VAR:new("gate", 0.0)
synth.env = SP:tenv("env", 0.08, 0, 0.9)
synth.randh = SP:randh("randh", -0.03, 0.03, 1000)
synth.noisefilt = SP:butlp("noisefilt", 1000)
synth.thresh = SP:thresh("thresh", 0.5, 2)
synth.rev = SP:revscm("rev", 0.97, 10000)
synth.trand = SP:trand("tr", 3000, 10000)
synth.delay = SP:delay("delay", 0.65, 1.1)
synth.butlp = SP:butlp("lpf", 4000)
synth.modal = SP:modal("mode")

synth.voice1 = SP:fosc("voice1", synth.sine, 440, 0.07, 1, 1, 0.8)
synth.voice2 = SP:fosc("voice2", synth.sine, 440, 0.07, 1, 1, 1.1)
synth.voice3 = SP:fosc("voice3", synth.sine, 440, 0.07, 1, 1, 0.8)

synth.port1 = SP:port("port1", 0.06)
synth.port2 = SP:port("port2", 0.08)
synth.port3 = SP:port("port3", 0.08)

synth.freq1 = VAR:new("freq1", 440)
synth.freq2 = VAR:new("freq2", 330)
synth.freq3 = VAR:new("freq3", 880)

synth.lfo1 = SP:osc("lfo1", synth.sine, 0.1, 1, 0)
synth.lfo2 = SP:osc("lfo2", synth.sine, 0.14, 1, 0.2)
synth.lfo3 = SP:osc("lfo3", synth.sine, 0.09, 1, 0.4)

function compute_voices(tmp1, tmp2, tmp3, met, osc)
    
    synth.port1:compute({synth.freq1, tmp3})
    synth.voice1:set("freq", tmp3)
    synth.voice1:compute({-1, osc})

    synth.lfo1:compute({-1, tmp3})
    SP:var_biscale(tmp3, 0, 1)
    SP:mul(osc, osc, tmp3)
    
    synth.port2:compute({synth.freq2, tmp3})
    synth.voice2:set("freq", tmp3)
    synth.voice2:compute({-1, tmp1})

    synth.lfo2:compute({-1, tmp3})
    SP:var_biscale(tmp3, 0, 1)
    SP:mul(tmp1, tmp1, tmp3)

    SP:add(osc, osc, tmp1)
    
    synth.port3:compute({synth.freq3, tmp3})
    synth.voice3:set("freq", tmp3)
    synth.voice3:compute({-1, tmp1})

    synth.lfo3:compute({-1, tmp3})
    SP:var_biscale(tmp3, 0, 1)
    SP:mul(tmp1, tmp1, tmp3)

    SP:add(osc, osc, tmp1)

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
    synth.randh:compute({-1, tmp})
    synth.noisefilt:compute({tmp, fm})
    synth.env:compute({tick, env})
    SP:mul(out, env, fm)
    -- Compute delay to tmp
    
    -- Modal collision to rev (not being used yet)
    modal = rev
    synth.modal:compute({-1, modal})
    -- Copy to temp variable and scale

    SP:var_copy(tmp, modal)
    SP:var_scale(tmp, 0.3)
    -- add collision back into dry (to be delayed) signal
    SP:add(out, out, tmp)

    del = env
    lpf = tenv 
    --- 
    synth.delay:compute({out, del})
    -- Filter delay to env (not being used yet)
    synth.butlp:compute({del, lpf})
    SP:var_scale(lpf, 0.9)
    -- add filtered delay back into dry signal
    SP:add(out, out, lpf)
    SP:add(out, out, modal)

    -- compute 4 fm oscilators, discard for now
    -- rev tick env not used, so use those 
    compute_voices(tick, fm, env, tenv, rev)
    SP:add(out, out, rev)

    --synth.rev:compute({out, out, rev, tmp})
    synth.rev:compute({out, rev})
    SP:var_scale(rev, 0.12)
    SP:add(out, out, rev)
    SP:output(out)
end, 10)

h:close()
c:close()

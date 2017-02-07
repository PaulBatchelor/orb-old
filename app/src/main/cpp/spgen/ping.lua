require("spgen.sp")

SP:nchan(1)

synth = {}
synth.sine = SP:ftbl("sine", 4096)
synth.gs = SP:gen_sine(synth.sine)
synth.tick = VAR:new("gate", 0.0)
synth.env = SP:tenvx("env", 0.001, 0.002, 0.001)
synth.fosc = SP:fosc("fm", synth.sine, 440, 0.4, 1, 1, 1)
synth.thresh = SP:thresh("thresh", 0.5, 2)
synth.rev = SP:revsc("rev", 0.97, 10000)
synth.trand = SP:trand("tr", 200, 4000)
synth.delay = SP:delay("delay", 0.7, 0.8)
synth.butlp = SP:butlp("lpf", 1000)

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

    -- use out since it's not being used yet
    synth.thresh:compute({synth.tick, tick})
    synth.trand:compute({tick, out})
    synth.fosc:set("freq", out)
    synth.fosc:compute({-1, fm})
    synth.env:compute({tick, env})
    SP:mul(out, env, fm)
    -- Compute delay to tmp
    synth.delay:compute({out, tmp})
    -- Filter delay to rev (not being used yet)
    synth.butlp:compute({tmp, rev})
    SP:var_scale(rev, 0.6)
    -- add filtered delay back into dry signal
    SP:add(out, out, rev)
    synth.rev:compute({out, out, rev, tmp})
    SP:var_scale(rev, 0.1)
    SP:add(out, out, rev)
    SP:output(out)
end, 10)

h:close()
c:close()

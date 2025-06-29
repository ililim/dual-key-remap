require "lua.dual-key-remap"

debug = true

remap.capslock.when_alone = escape
remap.capslock.with_other = ctrl

remap.rshift.when_alone = enter
remap.rshift.with_other = rshift

dump(remap)

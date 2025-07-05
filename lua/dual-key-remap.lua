function Key(name, opts)
    local key = {}
    key.name = name
    key.dir = opts and opts.dir or 0
    key.full_name = name
    if key.dir > 0 then
        key.full_name = key.full_name .. (key.dir == 1 and " (left)" or " (right)")
    end
    return key
end

local function autotable()
    return setmetatable({}, {
        __index = function(t, k)
            t[k] = autotable()
            return t[k]
        end
    })
end

function dump(t, indent)
    indent = indent or 0
    local spaces = string.rep("  ", indent)

    if type(t) ~= "table" then
        print(spaces .. tostring(t))
        return
    end

    print(spaces .. "{")
    for k, v in pairs(t) do
        local key = type(k) == "string" and k or "[" .. tostring(k) .. "]"
        if type(v) == "table" then
            print(spaces .. "  " .. key .. " = ")
            dump(v, indent + 1)
        else
            print(spaces .. "  " .. key .. " = " .. tostring(v))
        end
    end
    print(spaces .. "}")
end

escape = Key("escape")
ctrl = Key("ctrl")
enter = Key("enter")
rshift = Key("shift", { dir = 2 })
capslock = Key("capslock")


remap = autotable()

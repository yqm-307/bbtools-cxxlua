
function TreeEx(tb, nLevel)
    if tb == nil then
        return
    end

    if type(tb) == "table" then
        for key, value in pairs(tb) do
            print(key, ":")
            TreeEx(value, nLevel + 1)
        end
    else
        print(tb)
    end
end

function Tree(tb)
    for key, value in pairs(tb) do
        print(key, ":")
        TreeEx(value, 1)
    end
end

local GC = {__gc = function () print("gc once") end}

function NewPlayer()
    -- print(Player)
    -- print(type(Player))
    -- print(" tree ex: ")
    local pPlayer = Player:new("小明", 10)
    return pPlayer
end

function Test1()
    local pPlayer = NewPlayer()
    print("id ,name", pPlayer:GetId(), pPlayer:GetName())

    pPlayer:SetId(37);
    pPlayer:SetName("丽丽");
    print("id ,name", pPlayer:GetId(), pPlayer:GetName())
end

ObjectPool = {}

function GenerateObj()
    for i = 1, math.random(10000), 1 do
        table.insert(ObjectPool, NewPlayer())
    end
end

function ClearObj()
    ObjectPool = {}
end

function Test_GC()

    for i = 1, 100, 1 do
        GenerateObj()
        local nBytes = collectgarbage("count") * 1024
        print("", nBytes)
        ClearObj()
        collectgarbage("collect")
    end
end

function Main()
    Test1()
    print("--- 当前使用总量 ---")
    Test_GC()
end


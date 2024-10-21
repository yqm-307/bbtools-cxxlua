

function Notify()
    print(MsgQueue.GetMsg())
end

function Main()
    MsgQueue.SendMsg("hello i`am lua!")
end
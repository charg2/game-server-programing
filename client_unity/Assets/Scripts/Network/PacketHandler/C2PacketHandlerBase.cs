using JetBrains.Annotations;
using System;
using System.Diagnostics;
using System.Diagnostics.Tracing;
using UnityEngine;

public delegate void PacketHandlerFunc(PacketHeader header, C2PayloadVector payload, C2Session session);

public class C2PacketHandler
{
    public static PacketHandlerFunc[] handlers = new PacketHandlerFunc[(Int32)PacketType.PT_MAX];

    public C2PacketHandler()
    {
        for(int n = 0; n < (int)PacketType.PT_MAX; ++n)
        {
            handlers[n] = DoDefualutHandler;
        }
    }

    void DoDefualutHandler(PacketHeader header, C2PayloadVector payload, C2Session session)
    {
        UnityEngine.Debug.Log($"DoDefualutHandler : {header}");
        throw new NotImplementedException();
    }

    public PacketHandlerFunc this[PacketType type]
    {
        get 
        {
            if (PacketType.PT_NONE < type && type < PacketType.PT_MAX)
                return C2PacketHandler.handlers[(int)type];
            else
            {
                UnityEngine.Debug.Log($" IndexOutOfRangeException  Type : {type} ({(Int64)type}) ");
                throw new IndexOutOfRangeException();
            }
        }
    }
}



//public class LoginPacketHandler : C2PacketHandler
//{
//    public LoginPacketHandler() : base()
//    {
//        handlers[(Int32)PacketType.S2C_LOGIN_OK] = HiRequest;
//    }

//    void HiRequest(PacketHeader header, C2PayloadVector payload, C2Session session)
//    {
//        //HiPacket hi = new HiPacket();
//        //ByePacket bye = new ByePacket();

//        //payload.Read<HiPacket>(out hi);
//        //payload.Read<ByePacket>(out bye);
//    }
//}





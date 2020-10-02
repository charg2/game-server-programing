using System;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.SceneManagement;

public struct LoadedPlayerData
{
    public int level;
    public int hp;
    public int exp;
    public int y;
    public int x;
}


public class C2Client : Singleton<C2Client>
{
    public C2Session session;
    [SerializeField] MainPlayer player;
    public Int32 serverID { get; set; } = -1;
    public LoadedPlayerData PlayerData { get; set; }

    public string Nickname { get; set; } = "default";

    public C2Client(MainPlayer playerMovement)
    {
        session = C2Session.Instance;
        session.Client = this;
        player = playerMovement;

    }

    public void Start()
    {
        DontDestroyOnLoad(this);
        if (session == null)
        { 
            session = C2Session.Instance;
            session.Client = this;

        }
    }

    private void Update()
    {
    }

    //public void SendMovePakcet(MainPlayer player)
    //{
    //    cs_packet_move movePayload;//
    //    movePayload.header.size = (sbyte)Marshal.SizeOf<PacketHeader>();
    //    movePayload.header.type = PacketType.C2S_MOVE;
    //    movePayload.move_time = 0;
    //    movePayload.direction = player.Direction ;// player.direction;

    //    session.SendPacket<cs_packet_move>(movePayload);
    //}


    public void SendPakcet<T>(T packet)
    {
        session.SendPacket<T>(packet);
    }

    public MainPlayer Player
    {
        get
        {
            return player;
        }
        set
        {
            player = value;
        }
    }

    void OnSceneLoaded(Scene scene, LoadSceneMode mode)
    {
        if (scene.name == "1_Game_mmo")
        {
            //C2Session.Instance.gameObject.SetActive(true);
        }
    }


    private void OnEnable()
    {
        SceneManager.sceneLoaded += OnSceneLoaded;
    }
    void OnDisable()
    {
        SceneManager.sceneLoaded -= OnSceneLoaded;
    }

    /// <summary>
    //
    /// </summary>
    public unsafe void SendLoginPacket()
    {
        C2Session c2Session = C2Session.Instance;

        cs_packet_login loginPacket;
        loginPacket.header.type = PacketType.C2S_LOGIN;
        loginPacket.header.size = (byte)Marshal.SizeOf(typeof(cs_packet_login));

        byte[] unicodeByte = System.Text.Encoding.Unicode.GetBytes(C2Client.Instance.Nickname);
        //byte[] unicodeByte = System.Text.Encoding.Unicode.GetBytes(C2Client.Instance.Nickname);

        int nicknameLength = unicodeByte.Length > (int)Protocol.MAX_ID_LEN ? (int)Protocol.MAX_ID_LEN : unicodeByte.Length;
        Marshal.Copy(unicodeByte, 0, (IntPtr)loginPacket.name, nicknameLength);

        c2Session.SendPacket(loginPacket);
    }

    public unsafe void SendMovePacket(sbyte direction)
    {
        C2Session c2Session = C2Session.Instance;

        cs_packet_move movePayload;
        movePayload.header.type = PacketType.C2S_MOVE;
        movePayload.header.size = (byte)Marshal.SizeOf(typeof(cs_packet_move));
        movePayload.direction = direction;
        movePayload.move_time = 0;

        c2Session.SendPacket(movePayload);
    }


    public unsafe void SendAttackPacket()
    {
        C2Session c2Session = C2Session.Instance;

        cs_packet_attack attackPayload;
        attackPayload.header.type = PacketType.C2S_ATTACK;
        attackPayload.header.size = (byte)Marshal.SizeOf(typeof(cs_packet_attack));

        c2Session.SendPacket(attackPayload);
    }


    public unsafe void SendChatPacket(string msg)
    {
        C2Session c2Session = C2Session.Instance;

        cs_packet_chat chatPayload;
        chatPayload.header.type = PacketType.C2S_CHAT;
        chatPayload.header.size = (byte)Marshal.SizeOf(typeof(cs_packet_chat));


        byte[] chatBytes = System.Text.Encoding.Unicode.GetBytes(msg);
        int chatLength = chatBytes.Length > (int)Protocol.MAX_CHAT_LEN ? (int)Protocol.MAX_CHAT_LEN : chatBytes.Length;
        Marshal.Copy(chatBytes, 0, (IntPtr)chatPayload.chat, chatLength);

        chatPayload.id = C2Client.Instance.serverID;

        c2Session.SendPacket(chatPayload);
    }


    public unsafe void SendLogoutPacket()
    {
        C2Session c2Session = C2Session.Instance;

        cs_packet_logout logoutPayload;
        logoutPayload.header.type = PacketType.C2S_LOGOUT;
        logoutPayload.header.size = (byte)Marshal.SizeOf(typeof(cs_packet_logout));

        c2Session.SendPacket(logoutPayload);
    }
}

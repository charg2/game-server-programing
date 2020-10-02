using System;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using UnityEngine;

enum SessionState
{
    Uninitialized, // session 생성 이후 초기화 되지 않음.
    Initialized, 
    ConnectingToServer,
    Connected,
    Disconecting,
    Disconnected,
}

enum DisconnectReason
{
    None,
    DisconnectByServer,
    TimeoutDisconnect,
    Exception,
    DISCONECTING,
    DISCONNECTED,
}



public class C2Session : Singleton<C2Session>
{
    private Socket              socket;
    private C2PayloadVector     sendBuffer;
    private C2PayloadVector     recvBuffer;
    private Int64               recvBytes;
    private Int64               sendBytes;
    private IAsyncResult        retAsync;
    private SessionState        state       = SessionState.Uninitialized;
    public Int64               uniqueSessionId { get; set; }
    private Int32               reconnectCount;
    private C2PacketHandler     handler;     
    [SerializeField] C2Client   client;

    public C2Client Client 
    { 
        get
        {
            return client;
        }
        set
        {
            client = value;
        }
    }


    //public C2Session(C2Client client)
    //{
    //    OnInit();
    //    this.client = client;
    //}


    void Awake()
    {
        DontDestroyOnLoad(this);
        //OnInit();
    }

    void Update()
    {
        switch (state)
        {
            case SessionState.Uninitialized:
                break;

            case SessionState.Initialized:
                Connect();
                break;

            case SessionState.ConnectingToServer:
                ReConnect();
                break;

            case SessionState.Connected:
            {
                SendPayload();
                RecvPayload();
                break;
            }
            case SessionState.Disconecting:
                break;

            case SessionState.Disconnected:
                break;

            default:
                break;
        }
    }

    internal void SendPacket<T>(T packet)
    {
        sendBuffer.Wirte<T>(packet);
    }


    public void OnInit()
    {
        socket      = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        sendBuffer  = new C2PayloadVector();
        recvBuffer  = new C2PayloadVector();
        handler     = new LoginPacketHandler();

        socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.DontLinger, true);
        
        socket.Blocking = true;
        socket.NoDelay  = true;

        this.state = SessionState.Initialized;
    }

    public void ReConnect()
    {
        /// 여러번 시도해서 안되면 안되는거임.
    }

    public void Connect()
    {
        retAsync = socket.BeginConnect(Config.ServerIP, Config.ServerPort, new AsyncCallback(OnConnectComplete), this);
        
        this.state = SessionState.ConnectingToServer;

        return;
    }

    public void TryReconnect()
    {
        /// 여러번 시도해서 안되면 안되는거임.
        //retSync.IsCompleted;
        
        //reconnectCount += 1;
        if(reconnectCount == 5)
        {
            socket.EndConnect(retAsync);

            this.state = SessionState.Disconnected;
        }

        return;
    }

    public void Disconnect()
    {
        socket.BeginDisconnect(true, OnDisconnectComplete(), null);

        return;
    }

    public void SendPayload()
    {
        if(sendBuffer.Empty) // 보낼것 없으면...
        {
            return;
        }

        //Social,

        if (true == socket.Poll(0, SelectMode.SelectWrite)) // 데이터를 읽을 수 있다면 ...
        {
            int sentBytes = socket.Send(sendBuffer.GetBuffer(), sendBuffer.ReadHead, sendBuffer.Size, SocketFlags.None);
            sendBuffer.MoveReadHead(sentBytes);
            sendBuffer.Rewind();

            //Debug.Log($" send : { sentBytes}");
        }
    }

    public void RecvPayload()
    {
        if (true == socket.Poll(0, SelectMode.SelectRead)) // 데이터를 읽을 수 있다면 ...
        {
            Int32 receivedBytes = socket.Receive(recvBuffer.GetBuffer(), 0, recvBuffer.FreeSize, SocketFlags.None);
            //Int32 receivedBytes = socket.Receive(recvBuffer.GetBuffer(), recvBuffer.WriteHead, recvBuffer.FreeSize, SocketFlags.None, out error);
            if (0 < receivedBytes)
            {
                //Debug.Log($"recv bytes = { receivedBytes } bytes");
                
                recvBuffer.MoveWriteHead(receivedBytes);
                
                OnRecv();
            }
            else if (receivedBytes == 0)
            {
                Debug.LogError($"recv bytes = { receivedBytes } bytes");
            }
        }
    }


    int headerSize = Marshal.SizeOf<PacketHeader>();
    public void OnRecv()
    {
        PacketHeader header;

        for ( ; ; )
        {
            if (2 != recvBuffer.Peek(out header, 2))
                break;

            if (header.size > recvBuffer.Size)
                break;

            // 범위 체크..
            //Debug.Log($"{header.type} : {(int)header.type}, packet size : { header.size} ");

            handler[header.type](header, this.recvBuffer, this);

            //recveBuffer.MoveReadHead(header.size);
        }

        recvBuffer.Rewind();
    }

    public static unsafe void OnConnectComplete(IAsyncResult ar)
    {
        C2Session session = (C2Session)ar.AsyncState;

        Debug.Log($"OnConnectComplete : { session.socket.Connected }");

        if( session.socket.Connected == true)
        {
            session.state = SessionState.Connected;
            session.handler = new LoginPacketHandler();
        }
        else
        {
            if ( ++session.reconnectCount == 10) 
                session.state = SessionState.Disconnected;
            else
                session.state = SessionState.Initialized;
        }

    }

    public AsyncCallback OnDisconnectComplete()
    {
        AsyncCallback async = default;

        this.state = SessionState.Disconnected;

        this.handler = null;

        return async;
    }



    public void SetHandler(C2PacketHandler packetHandler)
    {
        this.handler = packetHandler;
    }

}

using System;
using System.Collections.Generic;
using System.Diagnostics;
using UnityEngine;
using UnityEngine.SceneManagement;

public class LoginPacketHandler : C2PacketHandler
{
    public LoginPacketHandler() : base()
    {
        handlers[(Int32)PacketType.S2C_LOGIN_OK] = OnLoginOk;
        handlers[(Int32)PacketType.S2C_LOGIN_FAIL] = OnLoginFail;
        handlers[(Int32)PacketType.S2C_LEAVE] = OnLeave;
    }


    // 로그인 확인.
    void OnLoginOk(PacketHeader header, C2PayloadVector payload, C2Session session)
    {
        sc_packet_login_ok loginOkPayload;

        payload.Read(out loginOkPayload);

        // user id 
        C2Session.Instance.uniqueSessionId = (Int64)loginOkPayload.id;
        C2Client.Instance.serverID = loginOkPayload.id;

        LoadedPlayerData playerData = new LoadedPlayerData();
        playerData.y = loginOkPayload.y;
        playerData.x = loginOkPayload.x;
        playerData.level = loginOkPayload.level;
        playerData.hp = loginOkPayload.hp;
        playerData.exp = loginOkPayload.exp;

        C2Client.Instance.PlayerData = playerData;
        //C2Client.Instance.PlayerData.y = .MoveCharacterUsingServerPosition(loginOkPayload.y, loginOkPayload.x);
        //C2Client.Instance.Player.Level = loginOkPayload.level;
        //C2Client.Instance.Player.SetHP(loginOkPayload.hp, loginOkPayload.level * 2);
        //C2Client.Instance.Player.Exp = loginOkPayload.exp;

        UIManager.Instance.CurrentState = UIState.Play;

        SceneManager.LoadSceneAsync("1_Game_mmo", LoadSceneMode.Single);

        // 핸들러 변경후 
        C2Session.Instance.SetHandler(new InGamePacketHandler());
        //C2Session.Instance.gameObject.SetActive(false);

    }

    void OnLoginFail(PacketHeader header, C2PayloadVector payload, C2Session session)
    {
        sc_packet_login_fail loginFailPayload;

        payload.Read(out loginFailPayload);

        UnityEngine.Debug.Log("Login failure!!");
        //C2Session.Instance.uniqueSessionId = (Int64)loginOkPayload.id;
    }


    // 로그인 씬에서 나감. 사실상 연결 끊기.
    void OnLeave(PacketHeader header, C2PayloadVector payload, C2Session session)
    {
        sc_packet_leave leavePayload;

        payload.Read(out leavePayload);
    }


    // login server to my client
    void ResponseLogin(PacketHeader header, C2PayloadVector payload, C2Session session)
    {
    }

    // 회원가입
    void ResponseRegistration(PacketHeader header, C2PayloadVector payload, C2Session session)
    {
    }

    // 로그인 씬에서 나감.
    void ResponseExitLogin(PacketHeader header, C2PayloadVector payload, C2Session session)
    {
    }

}
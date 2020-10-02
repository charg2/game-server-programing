using JetBrains.Annotations;
using LitJson;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Unity.IO;
using UnityEngine;

public class GameManager : Singleton<GameManager>
{
    private static GameManager          instance; // = new C2Network();
    private static string               gameVersion = "for testing";
    private static string               appVersion = "alpha";

    void Awake()
    {
        DontDestroyOnLoad(this);

        LoadConfigUsingJson();
    }

    private void LoadConfigUsingJson()
    {
        BetterStreamingAssets.Initialize();

        string[] paths = BetterStreamingAssets.GetFiles("\\", "*.txt", SearchOption.AllDirectories);

        //TextAsset jsonFile = Resources.Load("clientConfig") as TextAsset;

        for (int n = 0; n < paths.Length; ++n)
        {
            byte[] byteContents = BetterStreamingAssets.ReadAllBytes(paths[n]);
            string contentsString = System.Text.Encoding.UTF8.GetString(byteContents);

            if ("clientConfig.txt" == paths[n])
            {
                LoadClientConfig(contentsString);
            }
            else
            if ("serverConfig.txt" == paths[n])
            {
                LoadServerConfig(contentsString);
                C2Session.Instance.OnInit();
            }
            else
                Debug.Log($"zz : {paths[n]}");
        }
    }

    private void Update()
    {
    }


    public void LoadServerConfig(string serverConfigText)
    {
        ServerConfig serverConfig = JsonMapper.ToObject<ServerConfig>(serverConfigText);  // 맵퍼를 이용해서, 텍스트를 매핑. 

        Debug.Log($"{serverConfig.serverIP} : {serverConfig.serverPort}");

        Config.ServerIP = serverConfig.serverIP;
        Config.ServerPort = serverConfig.serverPort;
    }

    public void LoadClientConfig(string clientConfigText)
    {
        ClientConfig clientConfig = JsonMapper.ToObject<ClientConfig>(clientConfigText);  // 맵퍼를 이용해서, 텍스트를 매핑. 

        Config.ActionInterval = (float)clientConfig.actionInterval;
        Config.AppVersion = clientConfig.appVersion;

    }
}

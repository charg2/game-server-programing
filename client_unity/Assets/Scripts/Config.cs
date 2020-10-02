using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class ServerConfig
{
    public string   serverIP;
    public int      serverPort;
}

public class ClientConfig
{
    public string gameVersion;
    public string appVersion;
    public double actionInterval;
}



public static class Config
{
    public static string ServerIP { get; set; } = "127.0.0.1";
    public static int ServerPort { get; set; } = 21302;

    public static float ActionInterval { get; set; } = 1.0f;

    public static string GameVersion { get; set; } = "default";
    public static string AppVersion { get; set; } = "default";

}

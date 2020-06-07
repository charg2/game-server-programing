using System;
using System.Collections.Generic;
using System.Text;
using System.Xml.Serialization;

public static class Settings
{
    static Settings()
    {
        // Init Xml Parser & Load config.Xml
        //sectorMaximumCapacity = 0;

        sectorMaxNumberX  = 0;
        sectorMaxNumberY  = 0;
        sectorMaxNumberZ  = 0;

        sectorWidth   = 5.0f;
        sectorHeight  = 5.0f;
        sectorDepth   = 5.0f;
    }

#region WORLD
    //[XmlElement("World Offset" ,@default: 3 )]
    //public static int                   worldOffset = 1;
    //[XmlElement("World Offset", @default: 3)]
    //public static int                   sectorMaximumCapacity = 0;

    public static int sectorMaxNumberX { get; set; }
    public static int sectorMaxNumberY { get; set; }
    public static int sectorMaxNumberZ { get; set; }

    public static float sectorWidth { get; set; }
    public static float sectorHeight { get; set; }
    public static float sectorDepth { get; set; }
#endregion

#region NETWORK
    //[System.Xml.Serialization.XmlElement("Host Server Name")]
    //public static string            hostServerName = "HexaServer";

    //[XmlElement(key: "IP Address", @default: "127.0.0.1")]
    ////public static string                hostIpAddress = "14.38.228.211";// home
    //public static string            hostIpAddress = "127.0.0.1";// home

    //[XmlElement("Server Port")]
    //public static short             hostPort = 4532;//9092;
    //                                                //public static short                 hostPort = 4533;//home
    //[XmlElement("Nagle option")]
    //public static bool              enableNagleOption = true;
#endregion


}

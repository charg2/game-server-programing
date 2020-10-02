using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum UIState
{ 
    Login, Play, Chat
}


public class UIManager : Singleton<UIManager>
{
    [SerializeField] MainPlayer player;

    public UIState CurrentState { get; set;  } = UIState.Login;

    private void Awake()
    {
        DontDestroyOnLoad(this);

        Screen.SetResolution(1280, 1024, false);

    }

    public void Update()
    {

    }

}

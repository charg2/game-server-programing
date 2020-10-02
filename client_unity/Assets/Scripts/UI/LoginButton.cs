using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class LoginButton : MonoBehaviour
{
    // Start is called before the first frame update
    [SerializeField] InputField inputField;

    private void Awake()
    {
        //inputField = GetComponent<InputField>();
    }

    void Start()
    {
    
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Return) == true)
        {
            OnClick();
        }
    }
    
    public void OnClick()
    {
        //SceneManager.LoadSceneAsync("1_Game_mmo", LoadSceneMode.Single);
        Debug.Log(inputField.text);

        C2Client.Instance.Nickname = inputField.text;

        C2Client.Instance.SendLoginPacket();
        // login_packet.name; 
        //Debug.Log();
        // C2Session.Instance.SendPacket<cs_packet_enter>(enter_packet);
    }

}

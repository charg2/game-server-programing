using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.UIElements;


public enum MessageType
{
    System, User, Global, Friend, Party
}


[System.Serializable]
public class Message
{
    public string text;
    public Text textObject;
    public MessageType type;
}

public class ChatManager : Singleton<ChatManager>
{
    private int maxChatMessage = 25;

    [SerializeField] Text       instance;
    [SerializeField] InputField inputField;
    public GameObject           chatPannel;
    public GameObject           textObject;
    [SerializeField] Color user, system; 

    [SerializeField] List<Message> chatRecords ;


    void Start()
    {
        chatRecords = new List<Message>();
    }


    void Update()
    {
        switch( UIManager.Instance.CurrentState )
        {
            case UIState.Play:
            {
                if(Input.GetKeyDown(KeyCode.Return))
                {
                    inputField.Select();

                    UIManager.Instance.CurrentState = UIState.Chat;
                }

                break;
            }
            case UIState.Chat: 
            {
                if (Input.GetKeyDown(KeyCode.Return) )
                {
                    if (inputField.text != string.Empty) // 내 채팅 입력 
                    {
                        string chatMsg = $"{C2Client.Instance.Nickname} : {inputField.text}";

                        //AddChat(chatMsg, MessageType.User);
                        C2Client.Instance.SendChatPacket(chatMsg);
                    }

                    inputField.text = string.Empty;
                    UIManager.Instance.CurrentState = UIState.Play;
                }
                break;
            }
        }
    }


    public void AddChat(string text, MessageType type)
    {
        if (chatRecords.Count >= maxChatMessage)
        {
            Destroy(chatRecords[0].textObject.gameObject);
            chatRecords.Remove(chatRecords[0]);
        }

        Message newMessage = new Message();

        newMessage.text = text;

        GameObject newText = Instantiate(textObject, chatPannel.transform);

        newMessage.textObject = newText.GetComponent<Text>();

        newMessage.textObject.text = newMessage.text;
        newMessage.textObject.color = MessageTypeColor(type);

        chatRecords.Add(newMessage);
    }

    Color MessageTypeColor(MessageType type)
    {
        Color color;

        switch (type)
        {
            case MessageType.System:
                color = this.system;
                break;

            case MessageType.User:
                color = this.user;
                break;

            default:
                color = this.system;
                break;
        }

        return color;
    }

}


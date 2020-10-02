using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

public class NetMonoBehaviour : MonoBehaviour
{
    protected Rigidbody2D   myRigidbody;
    public long          ServerID { get{ return serverID; } set { serverID = value; } }

    [SerializeField] long serverID;
    [SerializeField] string nickname;
    [SerializeField] TextMesh textMesh;

    public string  Nickname 
    { 
        get 
        { 
            return nickname; 
        } 
        set 
        { 
            textMesh.text = value;
            nickname = value;
        } 
    }

    public void MoveToPositionUsingServerPostion(int y, int x)
    {
        Vector2 vector = new Vector2();
        vector.x = x;
        vector.y = y;

        try 
        {
            //myRigidbody.MovePosition(vector);
            gameObject.transform.position = vector;
        }
        catch(MissingReferenceException)
        {
            Debug.Log($"MissingReferenceException tag: {tag}");
        }
    }
}
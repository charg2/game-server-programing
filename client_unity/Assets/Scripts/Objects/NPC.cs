using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NPC : NetMonoBehaviour
{
    // Start is called before the first frame update

    public void Awake()
    {
        DontDestroyOnLoad(this);
        myRigidbody = GetComponent<Rigidbody2D>();
    }


    void Start()
    {
        myRigidbody = GetComponent<Rigidbody2D>();
    }

    private void OnEnable()
    {
        myRigidbody = GetComponent<Rigidbody2D>();
    }
}

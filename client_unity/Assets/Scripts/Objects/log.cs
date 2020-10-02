using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Log : Enemy
{
    public Transform    target;
    public float        chaseRadius;
    public float        attackRadius;
    public Transform    homePosition;


    public void Awake()
    {
        DontDestroyOnLoad(this);
        myRigidbody = GetComponent<Rigidbody2D>();
    }

    void Start()
    {
        myRigidbody = GetComponent<Rigidbody2D>();
    }

    // Update is called once per frame
    void Update()
    {
        //CheckDistance();
    }

    //void CheckDistance()
    //{
    //    float distance = Vector3.Distance(target.position, transform.position);

    //    if ( distance <= chaseRadius && distance > attackRadius)
    //    {
    //        transform.position = Vector3.MoveTowards(transform.position, target.position, moveSpeed * Time.deltaTime);
    //    }
    //}
}

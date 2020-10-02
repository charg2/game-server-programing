using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Tilemaps;
using UnityEngine.UI;
using UnityEngine.UIElements;

public class OtherPlayer : NetMonoBehaviour
{
    public PlayerState currentState;
    public float speed;
    private Vector3 change;
    private byte direction;
    private Animator animator;
    [SerializeField] TextMesh   nameTag;

    public int Level { get; set; } = 1;
    public int Exp { get; set; } = 0;
    public sbyte Direction { get; set; } = 0;


    public void Awake()
    {
        DontDestroyOnLoad(this);
    }

    void Start()
    {
        currentState = PlayerState.Walk;
        animator = GetComponent<Animator>();
        myRigidbody = GetComponent<Rigidbody2D>();

        animator.SetFloat("moveX", 0);
        animator.SetFloat("moveY", -1);

        nameTag.text = "default";
    }

    private void OnEnable()
    {
        myRigidbody = GetComponent<Rigidbody2D>();
    }
    // Update is called once per frame
    void Update()
    {
        // path finding 우선.
        //if (currentState != PlayerState.Attack && Input.GetButtonDown("attack"))
        //{
        //    StartCoroutine(AttackCo());
        //}
        //else if (currentState == PlayerState.Walk)
        //{
        //    UpdateAnimatorAndMove();
        //}
    }



    private IEnumerator AttackCo()
    {
        animator.SetBool("attacking", true);
        currentState = PlayerState.Attack;

        yield return null;

        animator.SetBool("attacking", false);

        yield return new WaitForSeconds(.3f);
        currentState = PlayerState.Walk;
    }

    private void UpdateAnimatorAndMove()
    {
        if (change != Vector3.zero)
        {
            //MoveCharacter();
            animator.SetFloat("moveX", change.x);
            animator.SetFloat("moveY", change.y);
            animator.SetBool("moving", true);
        }
        else
        {
            animator.SetBool("moving", false);
        }
    }

    
    //void MoveCharacter()
    //{
    //    //change.Normalize();
    //    myRigidbody.MovePosition(transform.position + change);
    //}


}
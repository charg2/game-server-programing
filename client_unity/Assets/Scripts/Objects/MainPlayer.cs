using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.Tilemaps;
using UnityEngine.UI;
using UnityEngine.UIElements;

public enum PlayerState
{
    Walk, Attack, Interact, PathFinding
}

public class MainPlayer : Singleton<MainPlayer>
{
    public PlayerState      currentState;
    public float            speed;
    private Rigidbody2D     myRigidbody;
    private Vector3         change;
    private byte            direction;
    private Animator        animator;
    private C2Client        client;

    private float             attackTimer = 1.0f;
    private float             movementTimer = 1.0f;

    public float Timer { get; set; } = .04f;

    public int Level { get; set; } = 1;
    public int Exp { get; set; } = 0;
    public sbyte Direction { get; set; } = 0;


    [SerializeField] Stat hp;
    [SerializeField] Stat exp;
    [SerializeField] Portrait portrait;
    [SerializeField] NameTag nameTag;
    private int prevLevel;

    void Awake()
    {
        DontDestroyOnLoad(this);

        hp.Initialize(150, 200);
        exp.Initialize(0, 200);
        portrait.SetLevel(1);

        currentState    = PlayerState.Walk;
        animator        = GetComponent<Animator>();
        myRigidbody     = GetComponent<Rigidbody2D>();

        animator.SetFloat("moveX", 0);
        animator.SetFloat("moveY", -1);

        // 처음에 안비추고 게임씬에서 비춤.
        //enabled = false;
    }

    // Update is called once per frame
    void Update()
    {
        if (UIManager.Instance.CurrentState != UIState.Play)
            return;

        change = Vector3.zero;

        CheckInputForMovement();
        CheckInputForAttack();
        UpdateAnimator();
    }

    private void CheckInputForAttack()
    {
        attackTimer += Time.deltaTime;
        if(attackTimer >= Config.ActionInterval)
        {
            if (currentState != PlayerState.Attack  && Input.GetKey(KeyCode.A) == true)
            {
                C2Client.Instance.SendAttackPacket();
                StartCoroutine(AttackCo());
                //animator.SetBool("attacking", true);
                attackTimer = 0.0f;
            }
            //if (currentState != PlayerState.Attack && Input.GetButtonDown("attack"))
            //{
            //    StartCoroutine(AttackCo());
            //    movementTimer = 0.0f;
            //}
            //else if (currentState == PlayerState.Walk)
            //{
            //    UpdateAnimatorAndMove();
            //    movementTimer = 0.0f;
            //}

        }
    }

    internal void ResetWhenResapwn()
    {
        hp.CurrentValue = hp.MaxValue;
        exp.CurrentValue = exp.CurrentValue / 2;
    }

    private void CheckInputForMovement()
    {
        movementTimer += Time.deltaTime;
        if (movementTimer >= Config.ActionInterval)
        {
            if (Input.GetKey(KeyCode.UpArrow) == true)
            {
                change.y = +1.0f;
                C2Client.Instance.SendMovePacket((sbyte)ServerDirection.Up);
                movementTimer = 0.0f;
                MoveCharacter();
            }
            else if (Input.GetKey(KeyCode.DownArrow) == true)
            {
                change.y = -1.0f;
                C2Client.Instance.SendMovePacket((sbyte)ServerDirection.Down);
                movementTimer = 0.0f;
                MoveCharacter();
            }
            if (Input.GetKey(KeyCode.LeftArrow) == true)
            {
                change.x = -1.0f;
                C2Client.Instance.SendMovePacket((sbyte)ServerDirection.Left);
                movementTimer = 0.0f;
                MoveCharacter();
            }
            else if (Input.GetKey(KeyCode.RightArrow) == true)
            {
                change.x = +1.0f;

                C2Client.Instance.SendMovePacket((sbyte)ServerDirection.Right);
                movementTimer = 0.0f;
                MoveCharacter();
            }
        }
    }

    private void OnEnable()
    {
        SceneManager.sceneLoaded += OnSceneLoaded;
    }
    void OnDisable()
    {
        SceneManager.sceneLoaded -= OnSceneLoaded;
    }

    void OnSceneLoaded(Scene scene, LoadSceneMode mode)
    {
        if ( scene.name == "1_Game_mmo")
        {
            C2Client.Instance.Player = this;
            //NetworkManager.Instance.Player = this;

            nameTag.Text.text = C2Client.Instance.Nickname;

            // stat
            hp.Initialize(C2Client.Instance.PlayerData.hp, 200);
            exp.Initialize(C2Client.Instance.PlayerData.exp,(int)( 100.0 * Math.Pow(2.0, (double)C2Client.Instance.PlayerData.level - 1)));
            portrait.SetLevel(C2Client.Instance.PlayerData.level);

            // 좌표
            MoveCharacterUsingServerPosition(-C2Client.Instance.PlayerData.y, C2Client.Instance.PlayerData.x);
        }
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

    private void UpdateAnimator()
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

    public void ParseLoginPacket( sc_packet_login_ok payload )
    {

    }

    void MoveCharacter()
    {
        //change.Normalize();
        myRigidbody.MovePosition(transform.position + change);
    }

    public void MoveCharacterUsingServerPosition(int y, int x)
    {
        Vector3 vector = new Vector3();
        vector.x = x;
        vector.y = y;

        gameObject.transform.position = vector;
    }


    public void SetStat(int level, int hp, int exp)
    {
        portrait.SetLevel(level);

        if(prevLevel != level)
        {
            this.prevLevel = level;
            this.exp.MaxValue = (int)(100.0 * Math.Pow(2.0, (double)level - 1));
            this.Level = level;
        }

        this.hp.CurrentValue = hp;
        this.exp.CurrentValue = exp;
    }

}

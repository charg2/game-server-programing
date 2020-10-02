using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Ogre : Enemy
{
    // Start is called before the first frame update

    private Animator anim;

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


    private void OnEnable()
    {
        myRigidbody = GetComponent<Rigidbody2D>();
        Attack();
    }

    void Update()
    {
    }

    public void Attack()
    {
        AttackCo();
    }
    public IEnumerator AttackCo()
    {
        //currentState = EnemyState.attack;
        anim.SetBool("attack", true);
        yield return new WaitForSeconds(0.5f);
        //currentState = EnemyState.walk;
        anim.SetBool("attack", false);
    }
}

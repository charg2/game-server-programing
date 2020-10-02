using System.Collections;
using System.Collections.Generic;
using UnityEngine;

enum EnemyType
{
    Peace, Agro
}


public class Enemy : NetMonoBehaviour
{
    public int              health;
    public int              baseAttack;
    public float            moveSpeed;
    private EnemyType       enemyType;


    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

}

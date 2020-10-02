using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class MapPosition : MonoBehaviour
{
    // Start is called before the first frame update

    [SerializeField] MainPlayer player;
    [SerializeField] Text text;


    void Start()
    {}

    // Update is called once per frame
    void Update()
    {
       text.text = $" ( {(int)player.gameObject.transform.position.x} , { (int)-player.gameObject.transform.position.y } )";    
    }


}

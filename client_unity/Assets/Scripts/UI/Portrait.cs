using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Portrait : MonoBehaviour
{
    // Start is called before the first frame update
    [SerializeField] Text levelText;

    void Start()
    {
        
    }

    public void SetLevel(int n)
    {
        levelText.text = $"{n}";
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class LoginInputField : MonoBehaviour
{
    // Start is called before the first frame update
    [SerializeField] InputField inputField;
    void Start()
    {
        inputField.Select();
    }
}

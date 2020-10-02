using System;
using System.Dynamic;
using UnityEngine;

public abstract class Singleton<T> : MonoBehaviour where T : MonoBehaviour
{
    private static T instance;

    public static T Instance
    {
        get
        {
            if (null == instance)
            {
                instance = (T)FindObjectOfType(typeof(T));
            }

            //if (instance == null)
            //{
            //    var singletonObject = new GameObject();
            //    instance = singletonObject.AddComponent<T>();
            //    singletonObject.name = typeof(T).ToString();
            //}

            return instance;
        }
    }
}

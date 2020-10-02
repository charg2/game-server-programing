using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.VFX;

public class ObjectPooler : Singleton<ObjectPooler>
{
    [SerializeField] GameObject[] prefabs;
    private Dictionary<string, GameObject> prefabIndexDict;
    private Dictionary<string, List<GameObject>> poolDict;

    public void Awake()
    {
        DontDestroyOnLoad(this);

        prefabIndexDict = new Dictionary<string, GameObject>();
        poolDict = new Dictionary<string, List<GameObject>>();
        for (int i = 0; i < prefabs.Length; i++)
        {
            var gameObjectList = new List<GameObject>();
            var prefab = prefabs[i];
            var go = Instantiate(prefab);

            prefabIndexDict.Add(prefab.name, prefabs[i]);
            go.SetActive(false);
            go.transform.position = Vector3.zero;

            gameObjectList.Add(go);
            poolDict.Add(prefab.name, gameObjectList);
        }
    }

    public GameObject Spawn(string gameObjectName, Vector3 position)
    {
        List<GameObject> goList;
        if ( false == poolDict.TryGetValue(gameObjectName, out goList))
        { 
            Debug.Log("없넌 오브젝트.");
            return null;
        }


        GameObject gobj;
        if (goList.Count == 0)
        {
            gobj = Instantiate(prefabIndexDict[gameObjectName]);
    
        }
        else 
        {
            gobj = goList[0];
            goList.RemoveAt(0);
        }

        gobj.SetActive(true);
        gobj.transform.position = position;

        return gobj;
    }


    public NetMonoBehaviour SpawnUsingTag(string gameObjectName, Vector3 position)
    {
        List<GameObject> goList;
        if (false == poolDict.TryGetValue(gameObjectName, out goList))
        {
            Debug.Log("없넌 오브젝트.");
            return null;
        }

        GameObject gobj;
        if (goList.Count == 0)
        {
            gobj = Instantiate(prefabIndexDict[gameObjectName]);
        }
        else
        {
            gobj = goList[0];
            goList.RemoveAt(0);
        }



        NetMonoBehaviour netMonoBehaviour = null;
        switch (gobj.tag)
        {
            case "NPC":
                netMonoBehaviour = gobj.GetComponent<NPC>();
                break;
            case "Player":
                netMonoBehaviour = gobj.GetComponent<OtherPlayer>();
                break;

            case "Log":
                netMonoBehaviour = gobj.GetComponent<Log>();
                break;

            case "Ogre":
                netMonoBehaviour = gobj.GetComponent<Ogre>();
                break;

            case "FixedLog":
                netMonoBehaviour = gobj.GetComponent<Log>();
                break;

            case "FixedOgre":
                netMonoBehaviour = gobj.GetComponent<Ogre>();
                break;

            default:
                Debug.Log($"NotFound Script in ObjectPool : {gobj.tag}");
                break;
        }
        

        netMonoBehaviour.gameObject.SetActive(true);
        netMonoBehaviour.gameObject.transform.position = position;

        return netMonoBehaviour;
    }



    public void Recycle(string gameObjectName, GameObject go)
    {
        go.SetActive(false);
        go.transform.position = Vector3.zero;
        
        try
        {
            switch (gameObjectName)
            {
                case "Ogre":
                    poolDict["Ogre"].Add(go);
                    break;

                case "Player":
                    poolDict["OtherPlayer"].Add(go);
                    break;

                case "NPC":
                    poolDict["NPC2"].Add(go);
                    break;

                case "Log":
                    poolDict["Log"].Add(go);
                    break;

                default:
                    break;
            }

        }
        catch(KeyNotFoundException)
        {
            Debug.Log($" KeyNotFoundExeception {gameObjectName}");
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

    public void OnSceneLoaded(Scene scene, LoadSceneMode mode)
    {
        //Debug.Log(mode);
        if (scene.name == "1_Game_mmo")
        {
            foreach(var goList in poolDict )
            {
                foreach(var gos in goList.Value)
                {
                    if (gos != null)
                    {
                        SceneManager.MoveGameObjectToScene(gos, scene);
                    }
                    else
                    {
                        Debug.Log("null in objPool");
                    }
                }
            }
        }
    }


    public void Recycle(string gameObjectName, NetMonoBehaviour go)
    {
        if(go == null)
        {
            return;
        }

        go.gameObject.SetActive(false);
        go.transform.position = Vector3.zero;

        try
        {
            switch (gameObjectName)
            {
                case "Ogre":
                    poolDict["Ogre"].Add(go.gameObject);
                    break;

                case "Player":
                    poolDict["OtherPlayer"].Add(go.gameObject);
                    break;

                case "NPC":
                    poolDict["NPC2"].Add(go.gameObject);
                    break;

                case "Log":
                    poolDict["Log"].Add(go.gameObject);
                    break;


                case "FixedLog":
                    poolDict["FixedLog"].Add(go.gameObject);
                    break;

                case "FixedOgre":
                    poolDict["FixedOgre"].Add(go.gameObject);
                    break;


                default:
                    break;
            }

        }
        catch (KeyNotFoundException)
        {
            Debug.Log($" KeyNotFoundExeception {gameObjectName}");
        }

    }

    //GameObject tt;

    //private void Update()
    //{
        //if(Input.GetKeyDown(KeyCode.X))
        //{
        //    tt =  ObjectPooler.Instance.Spawn( "NPC2", Vector3.zero);
        //}

        //if (Input.GetKeyDown(KeyCode.Z))
        //{
        //    ObjectPooler.Instance.Recycle("NPC2", tt);
        //}

    //}


}



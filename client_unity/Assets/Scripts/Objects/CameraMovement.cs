using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraMovement : MonoBehaviour
{
    public Transform    target;
    public float        smoothing;
    public Vector2      minPosition;
    public Vector2      maxPosition;

    // Start is called before the first frame update
    void Start()
    {
        minPosition.x += Camera.main.orthographicSize * 1.3f;
        minPosition.y += Camera.main.orthographicSize;

        maxPosition.x -= Camera.main.orthographicSize * 1.3f;
        maxPosition.y -= Camera.main.orthographicSize;
    }

    /// <summary>
    /// 주로 카메라는 LateUpdtea()를 통해 
    /// </summary>
    void LateUpdate()
    {
        UpdateCamera();
    }

    public void UpdateCamera()
    {
        if (transform.position != target.position)
        {
            // target z position follow camara z position.
            Vector3 targetPosition = new Vector3(target.position.x, target.position.y, transform.position.z);


            targetPosition.x = Mathf.Clamp(targetPosition.x, minPosition.x, maxPosition.x);
            targetPosition.y = Mathf.Clamp(targetPosition.y, minPosition.y, maxPosition.y);

            transform.position = Vector3.Lerp(transform.position, targetPosition, smoothing);
        }
    }

}

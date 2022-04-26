using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Pause : MonoBehaviour
{
    public List<Canvas> canvasesToToggle;

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            foreach (Canvas c in canvasesToToggle)
            {
                c.gameObject.SetActive(true);
            }

            this.gameObject.SetActive(false);

            if (Time.timeScale == 0)
                Time.timeScale = 1;
            else if (Time.timeScale == 1)
                Time.timeScale = 0;
        }
    }
}

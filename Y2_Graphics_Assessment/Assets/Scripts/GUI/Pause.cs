using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEditor;

public class Pause : MonoBehaviour
{
    public List<Canvas> canvasesToToggle;
    public ToggleButtonUse toggle;

    void Update()
    {
        if (Keyboard.current.escapeKey.wasPressedThisFrame)
        {
            PauseFunc();
            toggle.ToggleUse();
        }
    }

    public void PauseFunc()
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

    public void ExitFunc()
    {
        Application.Quit();
        if (EditorApplication.isPlaying)
            EditorApplication.ExitPlaymode();
    }
}

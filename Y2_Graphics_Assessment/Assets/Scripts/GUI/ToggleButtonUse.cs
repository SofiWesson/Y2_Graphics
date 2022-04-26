using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ToggleButtonUse : MonoBehaviour
{
    InputManagerTPS inputManager = null;

    bool canPressButtons = false;
    float timer = 0f;
    float timerReset = 0.2f;

    // Start is called before the first frame update
    void Start()
    {
        inputManager = GetComponent<InputManagerTPS>();
    }

    // Update is called once per frame
    void Update()
    {
        if (timer <= 0f)
        {
            if (Input.GetKey(KeyCode.Tab))
            {
                canPressButtons = !canPressButtons;
                inputManager.cursorLocked = !inputManager.cursorLocked;
                inputManager.cursorInputLook = !inputManager.cursorInputLook;
                
                if (canPressButtons)
                {
                    Cursor.lockState = CursorLockMode.None;
                    timer = timerReset;
                }
                else
                {
                    Cursor.lockState = CursorLockMode.Locked;
                    timer = timerReset;
                }
            }
        }

        if (timer > 0f)
            timer -= 1f * Time.deltaTime;
    }
}

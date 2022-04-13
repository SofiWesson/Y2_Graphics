using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

public class InputManagerTPS : MonoBehaviour
{
    // TPS inputs
    public Vector2 move;
    public Vector2 look;
    public bool jump;
    public bool aim;
    public bool shoot;
    public bool run;
    //public bool grenade;

    public bool movement;

    // mouse lockers
    public bool cursorLocked = true;
    public bool cursorInputLook = true;

    #region Functions We Call On Button Press

    public void MoveInput(Vector2 a_moveDirection)
    {
        move = a_moveDirection;
    }

    public void LookInput(Vector2 a_lookDirection)
    {
        look = a_lookDirection;
    }

    public void JumpInput(bool a_jumpState)
    {
        jump = a_jumpState;
    }

    public void AimInput(bool a_aimState)
    {
        aim = a_aimState;
    }

    public void ShootInput(bool a_shootState)
    {
        shoot = a_shootState;
    }

    public void RunInput(bool a_runState)
    {
        run = a_runState;
    }

    #endregion

    #region Functions Created For Connecting To Unity Input System

    public void OnMove(InputValue a_value)
    {
        MoveInput(a_value.Get<Vector2>());
    }

    public void OnLook(InputValue a_value)
    {
        if (cursorInputLook)
            LookInput(a_value.Get<Vector2>());
    }

    public void OnJump(InputValue a_value)
    {
        JumpInput(a_value.isPressed);
    }

    public void OnAim(InputValue a_value)
    {
        AimInput(a_value.isPressed);
    }

    public void OnShoot(InputValue a_value)
    {
        ShootInput(a_value.isPressed);
    }

    public void OnRun(InputValue a_value)
    {
        RunInput(a_value.isPressed);
    }

    #endregion

    private void OnApplicationFocus(bool focus)
    {
        SetCursorState(cursorLocked);
    }

    private void SetCursorState(bool a_state)
    {
        Cursor.lockState = a_state ? CursorLockMode.Locked : CursorLockMode.None;
    }
}

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : MonoBehaviour
{
    public void DoAction(BasicAction action)
    {
        Debug.Log("Doing " + action.actionName);
    }
}

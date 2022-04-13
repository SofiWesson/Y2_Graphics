using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ActionList : MonoBehaviour
{
    BasicAction[] _actions = null;

    public BasicAction[] actions
    {
        get
        {
            if (_actions == null)
                _actions = GetComponents<BasicAction>();
            return _actions;
        }
    }
}

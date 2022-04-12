using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class ActionUI : MonoBehaviour
{
    public BasicAction action;

    [Header("Child Components")]
    public Image icon;
    public TextMeshProUGUI nameTag;
    public TextMeshProUGUI descriptionTag;

    private void Start()
    {
        SetAction(action);
    }

    public void SetAction(BasicAction a)
    {
        action = a;
        if (nameTag)
            nameTag.text = action.actionName;
        if (descriptionTag)
            descriptionTag.text = action.description;
        if (icon)
        {
            icon.sprite = action.icon;
            icon.color = action.color;
        }
    }
}

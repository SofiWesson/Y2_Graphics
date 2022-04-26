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
    public GameObject actionObject;

    Player player;

    public void Init(Player p)
    {
        // store the player ref for use in our lambda function below
        player = p;

        /* find the button wherever we've placed it in the prefab
           for more complicated type of pregabs with multiple buttons, we'd make this a public memeber 
           and hook it up in the Unity editor */
        Button button = GetComponentInChildren<Button>();
        if (button)
            button.onClick.AddListener(() => { player.DoAction(action); });
    }

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
        actionObject = action.actionObj;
    }

    public void Action()
    {
        actionObject.BroadcastMessage("Action");
    }
}

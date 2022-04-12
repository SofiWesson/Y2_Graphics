using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ActionListUI : MonoBehaviour
{
    public ActionList actionList;
    public ActionUI prefab;

    // Start is called before the first frame update
    void Start()
    {
        Player player = actionList.GetComponent<Player>();
        foreach (BasicAction a in actionList.actions)
        {
            // make this a child of ours in creation. don't worry about specifying a positon as the LayoutGroup handles that

            ActionUI ui = Instantiate(prefab, transform);
            ui.SetAction(a);
            ui.Init(player);
        }
    }
}

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ChangeMatAction : MonoBehaviour
{
    MeshRenderer mesh;

    public Material mat1;
    public Material mat2;

    private void Awake()
    {
        mesh = GetComponent<MeshRenderer>();
        mesh.material = mat1;
    }

    public void Action()
    {
        string endName = " (Instance)";

        if (mesh.material.name == mat1.name + endName)
            mesh.material = mat2;
        else if (mesh.material.name == mat2.name + endName)
            mesh.material = mat1;
    }
}

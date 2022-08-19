using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ToggleParticleAction : MonoBehaviour
{
    new ParticleSystem particleSystem;

    private void Awake()
    {
        particleSystem = GetComponent<ParticleSystem>();
    }

    public void Action()
    {
        if (!particleSystem.isEmitting)
            particleSystem.Play();
        else
        {
            particleSystem.Stop();
        }
    }
}

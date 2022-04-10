using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class IncreaseBloom : MonoBehaviour
{
    private Renderer holoRenderer;
    public Vector2 bloomRange = new Vector2(10.0f, 20.0f);
    public float bloomBreathSpeed = 7;
    public float bloomIntensity = 10.0f;

    private WaitForSeconds loopWait = new WaitForSeconds(0.01f);

    bool isBloomIncreaseing = true;

    // Start is called before the first frame update
    void Awake()
    {
        holoRenderer = GetComponent<Renderer>();
    }

    // Update is called once per frame
    void Update()
    {
        
            if (isBloomIncreaseing)
                bloomIntensity += bloomBreathSpeed * Time.deltaTime;
            else
                bloomIntensity -= bloomBreathSpeed * Time.deltaTime;

            if (bloomIntensity <= bloomRange.x)
                isBloomIncreaseing = true;
            else if (bloomIntensity >= bloomRange.y)
                isBloomIncreaseing = false;

            holoRenderer.material.SetFloat("_Bloom", bloomIntensity);

    }
}
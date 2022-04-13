using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;
using Cinemachine;

public class AimOverrideControllerTPS : MonoBehaviour
{
    private CinemachineVirtualCamera m_aimCam = null;
    private float m_camSensitivity;
    private float m_aimCamSensitivity;
    private LayerMask m_aimLayerMask = new LayerMask();
    private Transform m_aimCamTransform;

    private Transform m_bulletSpawnPos;
    private GameObject m_bulletPrefab;

    private PlayerInput m_controller;
    private InputManagerTPS m_inputs;
    private Animator m_animator;

    private Vector2 m_currentAnimationVec;
    private Vector2 m_animationDirection;

    [Tooltip("How far can the bullet be shot for our hitscan")]
    public float shotDistance = 200f;

    // Start is called before the first frame update
    private void Awake()
    {
        m_controller = GetComponent<PlayerInput>();
        m_inputs = GetComponent<InputManagerTPS>();
        m_animator = GetComponent<Animator>();
    }

    // Update is called once per frame
    void Update()
    {
        Vector2 movement = m_inputs.move.normalized;
        m_currentAnimationVec = Vector2.SmoothDamp(m_currentAnimationVec, movement, ref m_animationDirection, 0.1f, 1f);

        m_animator.SetBool("IsStillADS", m_currentAnimationVec == Vector2.zero ? true : false);
        m_animator.SetBool("IsMotionADS", m_inputs.aim);
        m_animator.SetFloat("ForwardMotion", m_currentAnimationVec.y);
        m_animator.SetFloat("RightMotion", m_currentAnimationVec.x);

        Vector3 mouseWorldPos;
        Vector2 screenCenter = new Vector2(Screen.width / 2, Screen.height / 2);
        Ray ray = Camera.main.ScreenPointToRay(screenCenter);
        Transform hitRay = null;
        if (Physics.Raycast(ray, out RaycastHit raycastHit, shotDistance, m_aimLayerMask))
        {
            mouseWorldPos = raycastHit.point;
            hitRay = raycastHit.transform;
        }
        else
            mouseWorldPos = ray.GetPoint(20);
    
        if (m_inputs.aim)
        {
            
        }
    }
}

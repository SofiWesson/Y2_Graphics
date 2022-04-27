using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;
using Cinemachine;

public class AimOverrideControllerTPS : MonoBehaviour
{
    [Header("Camera Data")]
    [SerializeField]
    [Tooltip("Camera used for aiming, set to control")]
    private CinemachineVirtualCamera m_aimCam = null;
    [SerializeField]
    [Tooltip("Camera's standard sensitivity")]
    private float m_camSensitivity;
    [SerializeField]
    [Tooltip("Camera's sensitivity when aiming")]
    private float m_aimCamSensitivity;

    [Header("Layer Masking")]
    [SerializeField]
    [Tooltip("Layers to ignore")]
    private LayerMask m_aimLayerMask = new LayerMask();
    [Header("Bullet Data")]
    [SerializeField]
    [Tooltip("Provide a spawn position for a bullet")] // Shooting to be done
    private Transform m_bulletSpawnPos;
    [SerializeField]
    [Tooltip("Provide a bullet to be spawned")] // Shooting to be done

    private GameObject m_bulletPrefab;
    private PlayerControlsTPS m_controller;
    private InputManagerTPS m_inputs;
    private Animator m_animator;

    private Vector2 m_currentAnimationVec;
    private Vector2 m_animationDirection;

    [Space(10)]
    [Header("Right Hand Target Transforms")]
    public Transform RHTarget;
    private Vector3 m_RHTargetDefaultPosition = new Vector3(0, 0, 0);
    private Vector3 m_RHTargetDefaultRotation = new Vector3(0, 0, 0);
    public Vector3 RHTargetADSPosition = new Vector3(0, 0, 0);
    public Vector3 RHTargetADSRotation = new Vector3(0, 0, 0);

    [Space(10)]
    [Header("Right Hand Target Transforms")]
    public Transform LHTarget;
    private Vector3 m_LHTargetDefaultPosition = new Vector3(0, 0, 0);
    private Vector3 m_LHTargetDefaultRotation = new Vector3(0, 0, 0);
    public Vector3 LHTargetADSPosition = new Vector3(0, 0, 0);
    public Vector3 LHTargetADSRotation = new Vector3(0, 0, 0);

    [Space(20)]
    [Header("Other")]
    [Tooltip("How far can the bullet be shot for our hitscan")]
    public float shotDistance = 200f;

    [Tooltip("Use to control the rate at with the layers in the animation change.")]
    public float animationLayerTransitionRate = 10f;

    [Tooltip("Use to control how far it projects on a miss, useful for aiming via animation rigging")]
    public float missedRaycastDistance = 200f;

    public Transform m_aimMarkerTransform;

    // Start is called before the first frame update
    private void Awake()
    {
        m_controller = GetComponent<PlayerControlsTPS>();
        m_inputs = GetComponent<InputManagerTPS>();
        m_animator = GetComponent<Animator>();

        m_RHTargetDefaultPosition = RHTarget.localPosition;
        m_RHTargetDefaultRotation = RHTarget.localRotation.eulerAngles;

        m_LHTargetDefaultPosition = LHTarget.localPosition;
        m_LHTargetDefaultRotation = LHTarget.localRotation.eulerAngles;
    }

    // Update is called once per frame
    void Update()
    {
        Vector2 movement = m_inputs.move.normalized;
        m_currentAnimationVec = Vector2.SmoothDamp(m_currentAnimationVec,
            movement, ref m_animationDirection, 0.1f, 1f);
        m_animator.SetBool("IsStill", movement == Vector2.zero ? true : false);
        m_animator.SetBool("IsADS", m_inputs.aim);
        m_animator.SetFloat("ForwardMotion", m_currentAnimationVec.y);
        m_animator.SetFloat("RightMotion", m_currentAnimationVec.x);

        // make forwardmotion and right motion change faster

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
            mouseWorldPos = ray.GetPoint(missedRaycastDistance);
        if (m_inputs.aim)
        {
            m_aimCam.gameObject.SetActive(true);
            m_controller.SetCamSensitivity(m_aimCamSensitivity);
            m_controller.SetRotateOnMove(false);
            m_animator.SetLayerWeight(1, Mathf.Lerp(m_animator.GetLayerWeight(1),
                1f, Time.deltaTime * animationLayerTransitionRate));
            Vector3 aimTarget = mouseWorldPos;
            aimTarget.y = transform.position.y;
            Vector3 aimDirection = (aimTarget - transform.position).normalized;
            transform.forward = Vector3.Lerp(transform.forward, aimDirection, Time.deltaTime * 20);

            if (movement != new Vector2(0, 0))
            {
                RHTarget.localPosition = RHTargetADSPosition;
                RHTarget.localRotation = Quaternion.Euler(RHTargetADSRotation.x, RHTargetADSRotation.y, RHTargetADSRotation.z);

                LHTarget.localPosition = LHTargetADSPosition;
                LHTarget.localRotation = Quaternion.Euler(LHTargetADSRotation.x, LHTargetADSRotation.y, LHTargetADSRotation.z);
            }
            else
            {
                RHTarget.localPosition = m_RHTargetDefaultPosition;
                RHTarget.localRotation = Quaternion.Euler(m_RHTargetDefaultRotation.x, m_RHTargetDefaultRotation.y, m_RHTargetDefaultRotation.z);

                LHTarget.localPosition = m_LHTargetDefaultPosition;
                LHTarget.localRotation = Quaternion.Euler(m_LHTargetDefaultRotation.x, m_LHTargetDefaultRotation.y, m_LHTargetDefaultRotation.z);
            }

        }
        else
        {
            m_aimCam.gameObject.SetActive(false);
            m_controller.SetCamSensitivity(m_camSensitivity);
            m_controller.SetRotateOnMove(true);
            m_animator.SetLayerWeight(1, Mathf.Lerp(m_animator.GetLayerWeight(1),
                0f, Time.deltaTime * animationLayerTransitionRate));
        }
    }
}
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

[RequireComponent(typeof(CharacterController))]
public class PlayerControlsTPS : MonoBehaviour
{
    [Header("Player Movement")]
    [Tooltip("Movement speed of the player")]
    public float movementSpeed = 2.0f;
    [Tooltip("Sprinting speed of the player")]
    public float runningSpeed = 6.0f;
    [Tooltip("Player turn rate")]
    [Range(0f, 0.4f)]
    public float rotationSmoothRate = 0.1f;
    [Tooltip("Player rate of speed change")]
    public float speedChangeRate = 10.0f;

    [Space(10)]
    [Header("Player Grounded")]
    [Tooltip("Check if the player is grounded")]
    public bool isGrounded = true;
    [Tooltip("Rough ground offset, useful for complicated terrains")]
    [Range(-1, 1)]
    public float groundedOffset = -0.15f;
    [Tooltip("Radius of the above check")]
    public float groundedRadius = 0.3f;
    [Tooltip("Layers approved to be the ground")]
    public LayerMask groundLayers;

    [Space(10)]
    [Header("Player Cinemachine Camera Controls")]
    [Tooltip("Follow target set for the virtual camera that is active")]
    public GameObject cinemachineVirtualCamera;
    [Tooltip("In degrees, how high up it moves")]
    public float topClamp = 70.0f;
    [Tooltip("In degrees, how far down it moves")]
    public float bottomClamp = -30.0f;
    [Tooltip("Testing/overloading the currently accepted degrees")]
    public float cameraClampOverride;
    [Tooltip("Locking the camera on each axis")]
    public bool lockCamPosition;
    [Tooltip("Camera sensitivity speed adjustment")]
    public float cameraSensitivity;

    // Cinemachine Camera
    private float m_cinemachineTargetYaw;
    private float m_cinemachineTargetPitch;

    // Player grounded settings
    private float m_speed;
    private float m_animationBlend;
    private float m_rotation = 0;
    private float m_rotationVelcity;
    private float m_forwardVelocity;
    private bool m_rotateOnMove = true;

    private Animator m_animator;
    private CharacterController m_characterController;
    private InputManagerTPS m_input;
    private GameObject m_mainCamera;

    private bool m_hasAnimator;
    private float m_threshold = 0.01f;

    private void Awake()
    {
        if (m_mainCamera == null)
            m_mainCamera = GameObject.FindGameObjectWithTag("MainCamera");
    }

    // Start is called before the first frame update
    void Start()
    {
        m_hasAnimator = TryGetComponent(out m_animator);
        m_characterController = GetComponent<CharacterController>();
        m_input = GetComponent<InputManagerTPS>();
    }

    // Update is called once per frame
    void Update()
    {
        GroundCheck();
        Moving();
    }

    private void LateUpdate()
    {
        CameraRotation();
    }

    private void GroundCheck()
    {
        Vector3 spherePosition = new Vector3(transform.position.x, transform.position.y - groundedOffset, transform.position.z);
        isGrounded = Physics.CheckSphere(spherePosition, groundedRadius, groundLayers, QueryTriggerInteraction.Ignore);
        if (m_hasAnimator)
            m_animator.SetBool("IsGrounded", isGrounded);
    }

    private void CameraRotation()
    {
        if (m_input.look.sqrMagnitude >= m_threshold && !lockCamPosition)
        {
            m_cinemachineTargetYaw += m_input.look.x * Time.deltaTime * cameraSensitivity;
            m_cinemachineTargetPitch += m_input.look.y * Time.deltaTime * cameraSensitivity;
        }

        // next we want to make sure the camera is clamped
        m_cinemachineTargetYaw = ClampAngle(m_cinemachineTargetYaw, float.MinValue, float.MaxValue);
        m_cinemachineTargetPitch = ClampAngle(m_cinemachineTargetPitch, bottomClamp, topClamp);

        cinemachineVirtualCamera.transform.rotation = Quaternion.Euler(m_cinemachineTargetPitch + cameraClampOverride,
            m_cinemachineTargetYaw, 0f);
    }

    private void Moving()
    {
        // we need out speed to change depending on of the sprint is pressed or not
        float targetSpeed = m_input.run ? runningSpeed : movementSpeed;

        if (m_input.move == Vector2.zero)
            targetSpeed = 0f;

        // nedt we need to grab the players current speeds
        float currentHorizontalSpeed = new Vector3(m_characterController.velocity.x, 0f, m_characterController.velocity.z).magnitude;
        float speedOffset = 0.1f;
        float inputMag = m_input.movement ? m_input.move.magnitude : 1f;

        // now we adjust to the target speed
        if (currentHorizontalSpeed < targetSpeed - speedOffset ||
            currentHorizontalSpeed > targetSpeed + speedOffset)
        {
            m_speed = Mathf.Lerp(currentHorizontalSpeed, targetSpeed * inputMag, Time.deltaTime * speedChangeRate);
            m_speed = Mathf.Round(m_speed * 1000f) / 1000f; // this will keep it at 3 decimal places
        }
        else
        {
            m_speed = targetSpeed;
        }

        m_animationBlend = Mathf.Lerp(m_animationBlend, targetSpeed, Time.deltaTime * speedChangeRate);

        // we then need to normalise the input direction
        Vector3 inputDir = new Vector3(m_input.move.x, 0f, m_input.move.y).normalized;

        if (m_input.move != Vector2.zero)
        {
            m_rotation = Mathf.Atan2(inputDir.x, inputDir.z) * Mathf.Rad2Deg + m_mainCamera.transform.eulerAngles.y;
            float rotate = Mathf.SmoothDampAngle(transform.eulerAngles.y, m_rotation, ref m_rotationVelcity, rotationSmoothRate);
            if (m_rotateOnMove)
                transform.rotation = Quaternion.Euler(0f, rotate, 0f);
        }

        Vector3 targetDir = Quaternion.Euler(0f, m_rotation, 0f) * Vector3.forward;

        m_characterController.Move(targetDir.normalized * (m_speed * Time.deltaTime) +
            new Vector3(0f, m_forwardVelocity, 0f) * Time.deltaTime);

        if (m_hasAnimator)
        {
            m_animator.SetFloat("Speed", m_animationBlend);
            m_animator.SetFloat("SpeedMultiplier", inputMag);
        }
    }

    private static float ClampAngle(float a_angle, float a_min, float a_max)
    {
        if (a_angle < -360f)
            a_angle += 360;
        if (a_angle > 360f)
            a_angle -= 360;

        return Mathf.Clamp(a_angle, a_min, a_max);
    }

    private void OnDrawGizmosSelected()
    {
        Color transparentPurple = new Color(0.5f, 0.0f, 0.5f, 0.4f);
        Color transparenrRed = new Color(1, 0, 0, 0.4f);
        if (isGrounded)
            Gizmos.color = transparentPurple;
        else
            Gizmos.color = transparenrRed;

        Gizmos.DrawSphere(new Vector3(transform.position.x, transform.position.y - groundedOffset, transform.position.z), groundedRadius);
    }

    public void SetCamSensitivity(float a_camSensitivity)
    {
        cameraSensitivity = a_camSensitivity;
    }

    public void SetRotateOnMove(bool a_rotate)
    {
        m_rotateOnMove = a_rotate;
    }
}
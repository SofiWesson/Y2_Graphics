Shader "Custom/ForceField"
{
    Properties
    {
        _Color ("Color", Color) = (1,1,1,1)
        _MainTex ("Albedo (RGB)", 2D) = "white" {}
        _Glossiness ("Smoothness", Range(0,1)) = 0.5
        _Metallic ("Metallic", Range(0,1)) = 0.0
        _OutlineColor("Outline Color", Color) = (0, 0, 0, 1)
        _OutlineThickness("Outline Thickness", Range(0, 1)) = 0.1
        _OutlineTexture("Outline Texture", 2D) = "white" {}
        _Timer("Timer", Range(0, 1)) = 0
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 200

        CGPROGRAM
        // Physically based Standard lighting model, and enable shadows on all light types
        #pragma surface surf Standard fullforwardshadows

        // Use shader model 3.0 target, to get nicer looking lighting
        #pragma target 3.0

        sampler2D _MainTex;

        struct Input
        {
            float2 uv_MainTex;
        };

        half _Glossiness;
        half _Metallic;
        fixed4 _Color;

        // Add instancing support for this shader. You need to check 'Enable Instancing' on materials that use the shader.
        // See https://docs.unity3d.com/Manual/GPUInstancing.html for more information about instancing.
        // #pragma instancing_options assumeuniformscaling
        UNITY_INSTANCING_BUFFER_START(Props)
            // put more per-instance properties here
        UNITY_INSTANCING_BUFFER_END(Props)

        void surf (Input IN, inout SurfaceOutputStandard o)
        {
            // Albedo comes from a texture tinted by color
            fixed4 c = tex2D (_MainTex, IN.uv_MainTex) * _Color;
            o.Albedo = c.rgb;
            // Metallic and smoothness come from slider variables
            o.Metallic = _Metallic;
            o.Smoothness = _Glossiness;
            o.Alpha = c.a;
        }
        ENDCG

        Pass
        {
            Blend SrcAlpha OneMinusSrcAlpha
            ZWrite On
            CGPROGRAM

            #include "UnityCG.cginc"
            #pragma vertex vert
            #pragma fragment frag

            fixed4 _OutlineColor;
            float _OutlineThickness;
            sampler2D _OutlineTexture;
            float4 _OutlineTexture_ST;
            float _Timer;

            struct appdata
            {
                float4 vertex : POSITION;
                float4 normal : NORMAL;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float4 position : SV_POSITION;
                half3 normal : NORMAL;
                float2 uv : TEXCOORD0;
                float4 worldVertex : TANGENT;
            };

            v2f vert(appdata v)
            {
                v2f o;
                o.position = UnityObjectToClipPos(v.vertex + normalize(v.normal) * _OutlineThickness * (1 - _Timer));
                o.uv = TRANSFORM_TEX(v.uv, _OutlineTexture);
                o.uv.y -= _Time.y;
                o.normal = UnityObjectToWorldNormal(v.normal);
                o.worldVertex = mul(UNITY_MATRIX_M, v.vertex);
                return o;
            }

            fixed4 frag(v2f i) : SV_TARGET
            {
                half3 refl = 2 * dot(i.normal, _WorldSpaceLightPos0.xyz) * i.normal - _WorldSpaceLightPos0.xyz;
                half3 dir = normalize(_WorldSpaceCameraPos - i.worldVertex);
                float spec = max(0, dot(refl, dir));

                spec = pow(spec, 16);

                half rim = 1.0 - saturate(dot(dir, i.normal));
                return _OutlineColor * tex2D(_OutlineTexture, i.uv) + (spec + rim) * fixed4(1,1,1,1);
            }
            ENDCG
        }
    }
    FallBack "Diffuse"
}

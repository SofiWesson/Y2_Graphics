Shader "Custom/InvisibleMultipass"
{
    Properties
    {
        _Color ("Color", Color) = (1,1,1,1)
        _MainTex ("Albedo (RGB)", 2D) = "white" {}
        _Timer ("Timer", Range(0, 1)) = 0
    }
    SubShader
    {
        // this ensures we draw ourself after all opaque geometry
        Tags { "Queue"="Transparent" }

        // grabs the screen behind the objects, binding it into _BackgroundTexture
        GrabPass
        {
            "_BackgroundTexture"
        }


        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #include "UnityCG.cginc"
            #include "UnityLightingCommon.cginc"

            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
                float3 normal : NORMAL;
            };

            struct v2f
            {
                float2 uv : TEXCOORD0;
                half3 normal : NORMAL;
                UNITY_FOG_COORDS(1)
                float4 vertex : SV_POSITION;
                float4 worldVertex : TANGENT;
                float4 grabPos : TEXCOORD1;
            };

            sampler2D _MainTex;
            float4 _MainTex_ST;
            float4 _Color;
            half _Timer;

            v2f vert(appdata v)
            {
                v2f o;
                // this uses UnityObjectToClipPos from the Unity.CG.cginc to calculate the clip space of the vertex
                o.worldVertex = mul(UNITY_MATRIX_M, v.vertex);
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = TRANSFORM_TEX(v.uv, _MainTex);
                UNITY_TRANSFER_FOG(o, o.vertex);
                o.normal = UnityObjectToWorldNormal(v.normal);

                // we use ComputeGrabSCreenPos function from the UnityCG.cginc to grab the correct texture coordinate.
                o.grabPos = ComputeGrabScreenPos(o.vertex);
                return o;
            }

            sampler2D _BackgroundTexture;

            half4 frag(v2f i) : SV_Target
            {
                half4 bgColour = tex2Dproj(_BackgroundTexture, i.grabPos);
                half nl = max(0.2, dot(i.normal, _WorldSpaceLightPos0.xyz));
                fixed4 col = tex2D(_MainTex, i.uv) * _Color;
                half3 refl = 2 * dot(i.normal, _WorldSpaceLightPos0.xyz) * i.normal - _WorldSpaceLightPos0.xyz;
                half3 dir = normalize(_WorldSpaceCameraPos - i.worldVertex);
                float spec = max(0, dot(refl, dir));

                spec = pow(spec, 16);

                half rim = 1.0 - saturate(dot(dir, i.normal));
                UNITY_APPLY_FOG(i.fogCoord, col);
                half4 lambertColour = (col * nl) + spec * fixed4(1,1,1,1);
                half4 invisColour = bgColour + fixed4(rim,rim,rim,rim) + spec * fixed4(1,1,1,1);

                return (1 - _Timer) * invisColour + _Timer * lambertColour;
            }
            ENDCG
        }
    }
    FallBack "Diffuse"
}

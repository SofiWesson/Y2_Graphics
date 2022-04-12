Shader "Unlit/InvertOutline"
{
    Properties
    {
        _OutlineColor("Outline Color", Color) = (0, 0, 0, 1)
        _OutlineThickness("Outline Thickness", Range(0, 1)) = 0.1
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 100

        Pass
        {
            Cull Front

            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            // make fog work
            #pragma multi_compile_fog

            #include "UnityCG.cginc"

            struct appdata
            {
                float4 vertex : POSITION;
                float4 normal : NORMAL;
            };

            struct v2f
            {
                UNITY_FOG_COORDS(1)
                float4 vertex : SV_POSITION;
                half3 normal : NORMAL;
            };

            float4 _OutlineColor;
            float _OutlineThickness;

            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex + normalize(v.normal) * _OutlineThickness * 0.05);
                o.normal = UnityObjectToWorldNormal(v.normal);
                UNITY_TRANSFER_FOG(o,o.vertex);
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                fixed4 col = _OutlineColor;
                // apply fog
                UNITY_APPLY_FOG(i.fogCoord, col);
                return col;
            }
            ENDCG
        }
    }
}

Shader "Unlit/DotPattern"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _PatternTex("Pattern Texture", 2D) = "white" {}
        
        _HighlightColour("Highlight Colour", color) = (1, 1, 1, 1)
        _Colour("Colour", Color) = (1, 1, 1, 1)
        _ShadowColour("Shadow Colour", Color) = (0.5, 0.5, 0.5, 1)
        
        _Step1("Step 1", Range(0, 1)) = 0.2
        _Step2("Step 2", Range(0, 1)) = 0.4
        _Grandularity("Granularity", Range(20, 1000)) = 100
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 100

        Pass
        {
            Tags { "Light Mode" = "ForwardBase" }

            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            // make fog work
            #pragma multi_compile_fog

            #include "UnityCG.cginc"

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
                float4 screenPos : TEXCOORD1;
            };

            sampler2D _MainTex;
            float4 _MainTex_ST;

            sampler2D _PatternTex;
            float4 _PatternTex_ST;
            fixed4 _Colour;
            fixed4 _HighlightColour;
            fixed4 _ShadowColour;
            fixed _Grandularity;
            fixed _Step1;
            fixed _Step2;

            v2f vert (appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = TRANSFORM_TEX(v.uv, _MainTex);
                UNITY_TRANSFER_FOG(o,o.vertex);
                o.normal = UnityObjectToWorldNormal(v.normal);
                o.screenPos = ComputeScreenPos(o.vertex);
                
                return o;
            }

            fixed4 frag (v2f i) : SV_Target
            {
                // this is the diffuse lighting, grabbing a normal dot-product with the light direction.
                half nl = max(0, dot(i.normal, _WorldSpaceLightPos0.xyz));
                
                // sample the texture
                fixed4 col = tex2D(_MainTex, i.uv);
                
                // apply fog
                UNITY_APPLY_FOG(i.fogCoord, col);
                
                // grab the perspective divide to get the screen coordinates
                float2 wcoord = (_Grandularity * i.screenPos.xy / i.screenPos.w);

                // sample the pattern texture provided in the screen space
                fixed4 pattern = tex2D(_PatternTex, wcoord);

                // use a step function to choose between the highlighted, base or shadow colours
                float highlight = step(_Step2, 0.5 * (pattern + nl));
                float lighting = step(_Step1, 0.5 * (pattern + nl));
                
                return col * (_HighlightColour * highlight + (_Colour * lighting + _ShadowColour * (1 - lighting)) * (1 - highlight));
            }
            ENDCG
        }
    }

    Fallback "Diffuse"
}

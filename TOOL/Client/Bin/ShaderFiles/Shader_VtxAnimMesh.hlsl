matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

matrix g_BoneMatrices[512];

texture2D g_Texture;

vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);
vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);

//texture2D   g_DiffuseTexture;
vector g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal   : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent  : TANGENT;
    
    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vShade    : COLOR0;
    float fSpecular  : COLOR1;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    
    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x
                        + g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y 
	                    + g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z 
	                    + g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
    
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    
    vector vWorldPos = mul(vPosition, g_WorldMatrix);
    vector vWorldNormal = mul(vNormal, g_WorldMatrix);
    
    // Diffuse + Ambient
    Out.vShade = max(dot(normalize(g_vLightDir.xyz) * -1.f, normalize(vWorldNormal)), 0.f)
                 + (g_vLightAmbient * g_vMtrlAmbient);
    
    // Specular 
    vector vLook = vWorldPos - g_vCamPosition;
    vector vReflect = reflect(normalize(g_vLightDir), normalize(vWorldNormal));
    
    Out.fSpecular = pow(max(dot(normalize(vLook) * -1, normalize(vReflect)), 0.f), 30.f);
    
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vShade    : COLOR0;
    float fSpecular  : COLOR1;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (vDiffuse.a <= 0.3f)
        discard;
    
    //Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    // Diffuse + Ambient + Specular
    Out.vColor = vector(((g_vLightDiffuse * vDiffuse) * saturate(In.vShade)).rgb, 1.f)
                        + (g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular;
    
    if (0.3f >= Out.vColor.a)
        discard;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
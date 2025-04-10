Texture2D diffuseTexture : register(t0);
SamplerState textureSampler : register(s0);

cbuffer LightBuffer : register(b1) {
    float3 LightDirection;
    float LightIntensity;
    float3 AmbientColor;
    float AmbientIntensity;
}

struct PS_INPUT {
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 WorldPos : POSITION;
};

float4 main(PS_INPUT input) : SV_Target {
    // Sample texture
    float4 textureColor = diffuseTexture.Sample(textureSampler, input.TexCoord);
    
    // Normalize vectors for lighting calculation
    float3 normal = normalize(input.Normal);
    float3 lightDir = normalize(-LightDirection);
    
    // Calculate diffuse lighting
    float diffuseFactor = max(dot(normal, lightDir), 0.0f) * LightIntensity;
    
    // Combine lighting with texture and vertex color
    float4 finalColor = textureColor * input.Color;
    float3 ambient = AmbientColor * AmbientIntensity;
    float3 diffuse = finalColor.rgb * diffuseFactor;
    
    // Final color with lighting
    float3 color = ambient + diffuse;
    
    return float4(color, finalColor.a);
}

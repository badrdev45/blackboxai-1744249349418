cbuffer ConstantBuffer : register(b0) {
    matrix World;
    matrix View;
    matrix Projection;
}

struct VS_INPUT {
    float4 Position : POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT {
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 WorldPos : POSITION;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    
    // Transform position from object space to world space
    float4 worldPosition = mul(input.Position, World);
    
    // Transform to view space, then projection space
    output.Position = mul(worldPosition, View);
    output.Position = mul(output.Position, Projection);
    
    // Transform normal to world space
    output.Normal = mul(input.Normal, (float3x3)World);
    
    // Pass through color and texture coordinates
    output.Color = input.Color;
    output.TexCoord = input.TexCoord;
    
    // Store world position for lighting calculations
    output.WorldPos = worldPosition.xyz;
    
    return output;
}

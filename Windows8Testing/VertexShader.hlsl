cbuffer cbPerObject
{
    float4x4 gWorldViewProj;
};

struct VertexOut
{
    float4 PosH : POSITON;
    float4 Color : COLOR;
};

struct VertexIn
{
    float3 PosH : POSITION;
    float4 Color : COLOR;
};

VertexOut main(VertexIn vin)
{
    VertexOut vOut;

    vOut.PosH = mul(float4(vin.PosH, 1.0f), gWorldViewProj);

    vOut.Color = vin.Color;

    return vOut;
}
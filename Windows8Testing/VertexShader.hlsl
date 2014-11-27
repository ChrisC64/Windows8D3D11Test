cbuffer cbPerObject
{
    float4x4 gWorldViewProj;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

struct VertexIn
{
    float3 PosL : POSITION;
    float4 Color : COLOR;
};

VertexOut main(VertexIn vin)
{
    VertexOut vOut;

    vOut.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

    vOut.Color = vin.Color;

    return vOut;
}
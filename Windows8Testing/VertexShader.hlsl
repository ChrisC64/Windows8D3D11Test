//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(float4 Pos : POSITION, float4 Color : COLOR) 
{
    Pos.w = 1.0f;
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul(Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Color = Color;
    return output;
}
//cbuffer cbPerObject
//{
//    float4x4 gWorldViewProj;
//};
//
//struct VertexOut
//{
//    float4 PosH : SV_POSITION;
//    float4 Color : COLOR;
//};
//
//struct VertexIn
//{
//    float3 PosL : POSITION;
//    float4 Color : COLOR;
//};
//
//VertexOut main(VertexIn vin)
//{
//    VertexOut vOut;
//
//    vOut.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
//
//    vOut.Color = vin.Color;
//
//    return vOut;
//}

//float4 main( float4 pos : POSITION) : SV_POSITION
//{
//    return pos;
//}
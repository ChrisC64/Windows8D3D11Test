#include "Polygon_Cube.h"

Cube::Cube() : m_fPositionX(0.0f),
m_fPositionY(0.0f),
m_fPositionZ(0.0f),
m_fRotationX(0.0f),
m_fRotationY(0.0f),
m_fRotationZ(0.0f)
{   
}

Cube::~Cube()
{

}

void Cube::Init()
{
    // Assign vertices pos and color
    mVertices[0] = { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
    mVertices[1] = { -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
    mVertices[2] = { 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f };
    mVertices[3] = { 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
    mVertices[4] = { -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f };
    mVertices[5] = { -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f };
    mVertices[6] = { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    mVertices[7] = { 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };

    // Assign indices and prep for triangle list
    {
        // FRONT FACE
        mIndices[0] = 2; mIndices[1] = 0; mIndices[2] = 1;
        mIndices[3] = 0; mIndices[4] = 2; mIndices[5] = 3;

        // BACK FACE
        mIndices[6] = 6; mIndices[7] = 5; mIndices[8] = 4;
        mIndices[9] = 4; mIndices[10] = 7; mIndices[11] = 6;

        // LEFT FACE
        mIndices[12] = 1; mIndices[13] = 0; mIndices[14] = 5;
        mIndices[15] = 0; mIndices[16] = 4; mIndices[17] = 5;

        // RIGHT FACE
        mIndices[18] = 2; mIndices[19] = 6; mIndices[20] = 3;
        mIndices[21] = 7; mIndices[22] = 3; mIndices[23] = 6;

        // TOP FACE
        mIndices[24] = 6; mIndices[25] = 2; mIndices[26] = 5;
        mIndices[27] = 1; mIndices[28] = 5; mIndices[29] = 2;

        // BOTTOM FACE
        mIndices[30] = 7; mIndices[31] = 4; mIndices[32] = 0;
        mIndices[33] = 3; mIndices[34] = 7; mIndices[35] = 0;
    }
}

/////////////////////
// SET PARAMETERS //
////////////////////

void Cube::SetPosition(float x, float y, float z)
{
    m_fPositionX = x;
    m_fPositionY = y;
    m_fPositionZ = z;
}

void Cube::SetColor(float r, float g, float b, float a)
{
    // Range of r/g/b/a/ must be 0 >= x <= 1.0f
    if ((r >= 0.0f || r <= 1.0f) && (g >= 0.0f || g <= 1.0f) && (b >= 0.0f || b <= 1.0f) && (a >= 0.0f || a <= 1.0f))
    {
        for (unsigned int i = 0; i < sizeof(mVertices); i++)
        {
            mVertices[i].cr = r;
            mVertices[i].cg = g;
            mVertices[i].cb = b;
            mVertices[i].ca = a;
        }
    }
    else
    {
        for (unsigned int i = 0; i < sizeof(mVertices); i++)
        {
            mVertices[i].cr = 1.0f;
            mVertices[i].cg = 0;
            mVertices[i].cb = 0;
            mVertices[i].ca = 1.0f;
        }
    }
}

////////////////////
// GET PARAMETERS //
////////////////////

float Cube::GetPositionX()
{
    return m_fPositionX;
}

float Cube::GetPositionY()
{
    return m_fPositionY;
}

float Cube::GetPositionZ()
{
    return m_fPositionZ;
}

unsigned int Cube::GetSizeOfIndices()
{
    return (sizeof(mIndices)/sizeof(mIndices[0]));
}

unsigned int Cube::GetSizeOfVertexStruct()
{
    return sizeof(Vertex);
}

unsigned int Cube::GetSizeOfVertices()
{
    return (sizeof(mVertices)/sizeof(mVertices[0]));
}

void** Cube::GetPointerToVerticesArray()
{
    return (void**)mVertices;
}

void** Cube::GetPointerToIndicesArray()
{
    return (void**)mIndices;
}



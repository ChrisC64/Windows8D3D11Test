#include "GameObject_Cube.h"

GO_Cube::GO_Cube()
{

}

GO_Cube::~GO_Cube()
{
    
}

GO_Cube::GO_Cube(float x, float y, float z, bool isActive) :
m_pos(x, y, z),
m_id(0),
m_isActive(isActive)
{
}

void GO_Cube::Init()
{
    // Initialize the vertices of the cube
    m_Vertices[0].SetPosition(-1.0f, -1.0f, -1.0f);
    m_Vertices[0].SetColor(1.0f, 1.0f, 1.0f, 1.0f);

    m_Vertices[1].SetPosition(-1.0f, 1.0f, -1.0f); 
    m_Vertices[1].SetColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    m_Vertices[2].SetPosition(1.0f, 1.0f, -1.0f);
    m_Vertices[2].SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    
    m_Vertices[3].SetPosition(1.0f, -1.0f, -1.0f);
    m_Vertices[3].SetColor(0.0f, 1.0f, 0.0f, 1.0f);
    
    m_Vertices[4].SetPosition(-1.0f, -1.0f, 1.0f); 
    m_Vertices[4].SetColor(0.0f, 0.0f, 1.0f, 1.0f);

    m_Vertices[5].SetPosition(-1.0f, 1.0f, 1.0f);
    m_Vertices[5].SetColor(1.0f, 1.0f, 0.0f, 1.0f);

    m_Vertices[6].SetPosition(1.0f, 1.0f, 1.0f);
    m_Vertices[6].SetColor(0.0f, 1.0f, 1.0f, 1.0f);

    m_Vertices[7].SetPosition(1.0f, -1.0f, 1.0f);
    m_Vertices[7].SetColor(1.0f, 0.0f, 1.0f, 1.0f);

    // Initialize the indices of the cube object
    {
        // FRONT FACE
        m_indices[0] = 2; m_indices[1] = 0; m_indices[2] = 1;
        m_indices[3] = 0; m_indices[4] = 2; m_indices[5] = 3;

        // BACK FACE
        m_indices[6] = 6; m_indices[7] = 5; m_indices[8] = 4;
        m_indices[9] = 4; m_indices[10] = 7; m_indices[11] = 6;

        // LEFT FACE
        m_indices[12] = 1; m_indices[13] = 0; m_indices[14] = 5;
        m_indices[15] = 0; m_indices[16] = 4; m_indices[17] = 5;

        // RIGHT FACE
        m_indices[18] = 2; m_indices[19] = 6; m_indices[20] = 3;
        m_indices[21] = 7; m_indices[22] = 3; m_indices[23] = 6;

        // TOP FACE
        m_indices[24] = 6; m_indices[25] = 2; m_indices[26] = 5;
        m_indices[27] = 1; m_indices[28] = 5; m_indices[29] = 2;

        // BOTTOM FACE
        m_indices[30] = 7; m_indices[31] = 4; m_indices[32] = 0;
        m_indices[33] = 3; m_indices[34] = 7; m_indices[35] = 0;
    }
}

void GO_Cube::Draw(float dt)
{
    //m_pD3dRender->Render(dt);
}

//bool GO_Cube::InitRenderer(Render::D3DRender *pRender)
//{
//    if (!pRender)
//        return false;
//    m_pD3dRender = pRender;
//    if (m_pD3dRender)
//        return true;
//    else
//        return false;
//}

void GO_Cube::SetPosition(Position *pPos)
{
    m_pos.x = pPos->x;
    m_pos.y = pPos->y;
    m_pos.z = pPos->z;
}

void GO_Cube::SetPosition(float x, float y, float z)
{
    m_pos.x = x;
    m_pos.y = y;
    m_pos.z = z;
}

void GO_Cube::SetIsActive(bool isActive)
{
    m_isActive = isActive;
}

void GO_Cube::SetId(int id)
{
    m_id = id;
}

GO_Cube::Position* GO_Cube::GetPosition()
{
    return &m_pos;
}
float GO_Cube::GetPosX()
{
    return m_pos.x;
}

float GO_Cube::GetPosY()
{
    return m_pos.y;
}

float GO_Cube::GetPosZ()
{
    return m_pos.z;
}

bool GO_Cube::IsActive()
{
    return m_isActive;
}

int GO_Cube::GetId()
{
    return m_id;
}

// Obtain information on the arrays of our class
unsigned int GO_Cube::GetSizeOfIndices()
{
    return sizeof(unsigned int);
}

unsigned int GO_Cube::GetSizeOfVertices()
{
    return sizeof(Point::Vertex);
}

unsigned int GO_Cube::GetSizeOfVertexArray()
{
    return sizeof(m_Vertices) / sizeof(m_Vertices[0]);
}

unsigned int GO_Cube::GetSizeOfIndexArray()
{
    return sizeof(m_indices) / sizeof(m_indices[0]);
}

void** GO_Cube::GetPointerToVerticesArray()
{
    return (void**)m_Vertices;
}

void** GO_Cube::GetPointerToIndexArray()
{
    return (void**)m_indices;
}
#include "GameObject_Cube.h"

GO_Cube::GO_Cube() :
m_Pos(0, 0, 0),
m_Scale(1, 1, 1),
m_Rotation(0, 0, 0),
m_id(0),
m_isActive(true)
{
    Init();
}

GO_Cube::~GO_Cube()
{
    
}

GO_Cube::GO_Cube(float posx, float posy, float posz,
    float scalex, float scaley, float scalez,
    float rotx, float roty, float rotz,
    bool isActive) :
m_Pos(posx, posy, posz),
m_Scale(scalex, scaley, scalez),
m_Rotation(rotx, roty, rotz),
m_id(0),
m_isActive(isActive)
{
    Init();
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

// SET METHODS //
void GO_Cube::SetPosition(Vector3 *pPos)
{
    m_Pos.x = pPos->x;
    m_Pos.y = pPos->y;
    m_Pos.z = pPos->z;
}

void GO_Cube::SetPosition(float x, float y, float z)
{
    m_Pos.x = x;
    m_Pos.y = y;
    m_Pos.z = z;
}

void GO_Cube::SetIsActive(bool isActive)
{
    m_isActive = isActive;
}

void GO_Cube::SetId(int id)
{
    m_id = id;
}

void GO_Cube::SetScale(float x, float y, float z)
{
    m_Scale.x = x;
    m_Scale.y = y;
    m_Scale.z = z;
}

void GO_Cube::SetRotation(float roll, float pitch, float yaw)
{
    m_Rotation.x = roll;
    m_Rotation.y = pitch;
    m_Rotation.z = yaw;
}
// GET METHODS //
GO_Cube::Vector3* GO_Cube::GetPosition()
{
    return &m_Pos;
}
float GO_Cube::GetPosX()
{
    return m_Pos.x;
}

float GO_Cube::GetPosY()
{
    return m_Pos.y;
}

float GO_Cube::GetPosZ()
{
    return m_Pos.z;
}

bool GO_Cube::IsActive()
{
    return m_isActive;
}

int GO_Cube::GetId()
{
    return m_id;
}

float GO_Cube::GetRotX()
{
    return m_Rotation.x;
}

float GO_Cube::GetRotY()
{
    return m_Rotation.y;
}

float GO_Cube::GetRotZ()
{
    return m_Rotation.z;
}

float GO_Cube::GetScaleX()
{
    return m_Scale.x;
}

float GO_Cube::GetScaleY()
{
    return m_Scale.y;
}

float GO_Cube::GetScaleZ()
{
    return m_Scale.z;
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
    return sizeof(m_Vertices);
}

unsigned int GO_Cube::GetSizeOfIndexArray()
{
    return sizeof(m_indices);
}

void** GO_Cube::GetPointerToVerticesArray()
{
    return (void**)m_Vertices;
}

void** GO_Cube::GetPointerToIndexArray()
{
    return (void**)m_indices;
}
#pragma once
#include "pch.h"
#include "GameObject.h"
#include "Vertex.h"
#define SAFE_RELEASE(x) if(x){x->Release(); x = nullptr;}
//using namespace Object;
class GO_Cube : public Object::GameObject
{
private:
    struct Vector
    {
        float x;
        float y;
        float z;
        Vector()
        {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
        }
        Vector(float px, float py, float pz)
        {
            x = px;
            y = py;
            z = pz;
        }
        Vector(const Vector& pos)
        {
            (*this) = pos;
        }
        Vector(Vector *p)
        {
            this->x = p->x;
            this->y = p->y;
            this->z = p->z;
        }
        Vector& operator=(const Vector& p)
        {
            this->x = p.x;
            this->y = p.y;
            this->z = p.z;

            return *this;
        }
    };

protected:
    //Position m_Position;
    Point::Vertex m_Vertices[8];
    Vector m_Pos;
    Vector m_Scale;
    Vector m_Rotation;
    unsigned int m_indices[36];
    unsigned int m_id;
    bool    m_isActive;
    // Render object
public:

    // Constructor
    GO_Cube();
    ~GO_Cube();
    // Override
    GO_Cube(
        float posx, float posy, float posz,
        float scalex, float scaley, float scalez,
        float rotx, float roty, float rotz,
        bool isActive);

    //GO_Cube(Point::Vertex *pVertex, D3DRender* pRender);
    // Initialize Objects referenced by this class
    //bool InitRenderer(Render::D3DRender *pRender);
    
    // Asbtract Class Methods Definitions
    void Draw(float dt);
    void Init();
    
    // Set Methods //
    void SetPosition(Vector* pPosition);
    void SetPosition(float x, float y, float z);
    void SetIsActive(bool isActive);
    void SetId(int id);
    void SetScale(float x, float y, float z);
    void SetRotation(float rill, float pitch, float yaw);

    // Get Methods //
    Vector* GetPosition();
    float GetPosX();
    float GetPosY();
    float GetPosZ();
        // Rotation // 
    float GetRotX();
    float GetRotY();
    float GetRotZ();
        // Scale //
    float GetScaleX();
    float GetScaleY();
    float GetScaleZ();
        // Set Methods //
    bool IsActive();
    int GetId();

    // information on the data structures to return
    unsigned int GetSizeOfIndices();
    unsigned int GetSizeOfVertices();
    unsigned int GetSizeOfVertexArray();
    unsigned int GetSizeOfIndexArray();
    void** GetPointerToVerticesArray(); // Should be const()?
    void** GetPointerToIndexArray(); // should be const? 
};
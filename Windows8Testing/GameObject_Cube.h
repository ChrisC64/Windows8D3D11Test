#pragma once
#include "pch.h"
#include "GameObject.h"
#include "Vertex.h"
#define SAFE_RELEASE(x) if(x){x->Release(); x = nullptr;}
//using namespace Object;
class GO_Cube : public Object::GameObject
{
private:
    struct Vector3
    {
        float x;
        float y;
        float z;
        Vector3()
        {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
        }
        Vector3(float px, float py, float pz)
        {
            x = px;
            y = py;
            z = pz;
        }
        Vector3(const Vector3& pos)
        {
            (*this) = pos;
        }
        Vector3(Vector3 *p)
        {
            this->x = p->x;
            this->y = p->y;
            this->z = p->z;
        }
        Vector3& operator=(const Vector3& p)
        {
            this->x = p.x;
            this->y = p.y;
            this->z = p.z;

            return *this;
        }
    };

    Point::Vertex m_Vertices[8];
    unsigned int m_indices[36];
    Vector3 m_Pos;
    Vector3 m_Scale;
    Vector3 m_Rotation;
    bool    m_isActive;
    unsigned int m_id;

    DirectX::XMMATRIX m_mTransMat;
    DirectX::XMMATRIX m_mScaleMat;
    DirectX::XMMATRIX m_mRotMat;

protected:
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

    // Initialize Objects referenced by this class
    //bool InitRenderer(Render::D3DRender *pRender);
    
    // Asbtract Class Methods Definitions
    void Draw(float dt);
    void Init();
    
    // Set Methods //
    void SetPosition(Vector3* pPosition);
    void SetPosition(float x, float y, float z);
    void SetIsActive(bool isActive);
    void SetId(int id);
    void SetScale(float x, float y, float z);
    void SetRotation(float rill, float pitch, float yaw);

    // Get Methods //
    Vector3* GetPosition();
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

    //DirectX::XMMATRIX& GetScaleMat();
    //DirectX::XMMATRIX& GetTransMat();
    //DirectX::XMMATRIX& GetRotMat();
    //DirectX::XMMATRIX& GetWorldMat();
    //void SetScaleMat(float x, float y, float z);
    //void SetTransMat(float x, float y, float z);
    //void SetRotMat(float );

};
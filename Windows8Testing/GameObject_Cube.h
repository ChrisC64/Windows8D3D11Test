#pragma once
#include "pch.h"
#include "GameObject.h"
#include "Vertex.h"
#define SAFE_RELEASE(x) if(x){x->Release(); x = nullptr;}
//using namespace Object;
class GO_Cube : public Object::GameObject
{
private:
    struct Position
    {
        float x;
        float y;
        float z;
        Position()
        {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
        }
        Position(float px, float py, float pz)
        {
            x = px;
            y = py;
            z = pz;
        }
        Position(const Position& pos)
        {
            (*this) = pos;
        }
        Position(Position *p)
        {
            this->x = p->x;
            this->y = p->y;
            this->z = p->z;
        }
        Position& operator=(const Position& p)
        {
            this->x = p.x;
            this->y = p.y;
            this->z = p.z;

            return *this;
        }
    };

protected:
    //Position m_position;
    Point::Vertex m_Vertices[8];
    Position m_pos;
    unsigned int m_indices[36];
    unsigned int m_id;
    bool    m_isActive;
    // Render object
public:

    // Constructor
    GO_Cube();
    ~GO_Cube();
    // Override
    GO_Cube(float x, float y, float z, bool isActive);

    //GO_Cube(Point::Vertex *pVertex, D3DRender* pRender);
    // Initialize Objects referenced by this class
    //bool InitRenderer(Render::D3DRender *pRender);
    
    // Asbtract Class Methods Definitions
    void Draw(float dt);
    void Init();
    
    // Set Params
    void SetPosition(Position* pPosition);
    void SetPosition(float x, float y, float z);

    void SetIsActive(bool isActive);
    void SetId(int id);

    // Get Params
    Position* GetPosition();
    float GetPosX();
    float GetPosY();
    float GetPosZ();

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
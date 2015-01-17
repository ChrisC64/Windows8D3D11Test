#pragma once
#include <iostream>
namespace Object
{
    class GameObject
    {
    protected:
        //struct Position
        //{
        //    float x;
        //    float y;
        //    float z;
        //    Position()
        //    {
        //        x = 0.0f;
        //        y = 0.0f;
        //        z = 0.0f;
        //    }
        //    Position(float px, float py, float pz)
        //    {
        //        x = px;
        //        y = py;
        //        z = pz;
        //    }
        //};
        //Point::Vertex mVertex;
        //unsigned int mId;
        //bool    mIsActive;

        //D3DRender *m_pD3dRender;
        
    public:
        GameObject();
        ~GameObject();
        //virtual void Render() = 0; 
        virtual void Init() = 0;
        
        //bool IsActive();

        //////////////////////
        //// GET PARAMETERS //
        //////////////////////
        //float GetPositionX();
        //float GetPositionY();
        //float GetPositionZ();
        //Position* GetPosition();

        ////////////////////
        // SET PARAMETERS //
        ////////////////////
        //void SetPosition(Point::Vertex::Position *v);
        //void SetPosition(float x, float y, float z);
        //void SetIsActive();

        /*  Name: Draw
            Param: Float
            Return: void
            Info: The object's draw call to be made 
        */
        virtual void Draw(float dt) = 0;

    };
}
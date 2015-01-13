#pragma once
#include <iostream>
#include "Direct3DRender.h"
namespace Object
{
    class GameObject : public D3DRender
    {
    private:
        struct Position
        {
            float x;
            float y;
            float z;
        };

        unsigned int mId;
        bool    mIsActive;

        D3DRender *m_pD3dRender;
        
    public:
        GameObject();
        ~GameObject();
        virtual void Render() = 0; 
        virtual void Init() = 0;
        
        bool IsActive();

        ////////////////////
        // GET PARAMETERS //
        ////////////////////
        float GetPositionX();
        float GetPositionY();
        float GetPositionZ();

        ////////////////////
        // SET PARAMETERS //
        ////////////////////
        void SetPosition(Position *v);
        void SetPosition(float x, float y, float z);
        void SetIsActive();

        /*  Name: Draw
            Param: Float
            Return: void
            Info: The object's draw call to be made 
        */
        virtual void Draw(float dt, D3DRender* pD3DRender) = 0;

    };
}
#pragma once
#include <iostream>
namespace Object
{
    class GameObject
    {
    protected:
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
        };
        
    public:
        GameObject();
        ~GameObject();

        virtual void Init() = 0;        
        virtual bool IsActive() = 0;

        // Position //
        virtual float GetPosX() = 0;
        virtual float GetPosY() = 0;
        virtual float GetPosZ() = 0;

        // Rotation // 
        virtual float GetRotX() = 0;
        virtual float GetRotY() = 0;
        virtual float GetRotZ() = 0;
        // Scale //
        virtual float GetScaleX() = 0;
        virtual float GetScaleY() = 0;
        virtual float GetScaleZ() = 0;
        // Set Methods //
        virtual void SetPosition(float x, float y, float z) = 0;
        virtual void SetIsActive(bool isActive) = 0;
        virtual void SetScale(float x, float y, float z) = 0;
        virtual void SetRotation(float yaw, float pitch, float roll) = 0;
        // information on the data structures to return
        virtual unsigned int GetSizeOfIndices() = 0;
        virtual unsigned int GetSizeOfVertices() = 0;
        virtual unsigned int GetSizeOfVertexArray() = 0;
        virtual unsigned int GetSizeOfIndexArray() = 0;
        virtual void** GetPointerToVerticesArray() = 0; // Should be const()?
        virtual void** GetPointerToIndexArray() = 0; // should be const? 


        /*  Name: Draw
            Param: Float
            Return: void
            Info: The object's draw call to be made 
        */
        virtual void Draw(float dt) = 0;

    };
}
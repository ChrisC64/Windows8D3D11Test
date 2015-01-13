#ifndef POLYGON_CUBE_H
#define POLYGON_CUB_H
#include <iostream>
#include <DirectXMath.h>
class Cube 
{
private:
    struct Vertex
    {
        float x, y, z; // position - maybe include w? scale
        float cr, cb, cg, ca; // color values

        Vertex& operator =(const Vertex& o)
        {
            x = o.x;
            y = o.y;
            z = o.z;
            cr = o.cr;
            cg = o.cg;
            cb = o.cb;
            ca = o.ca;

            return *this;
        }
    };

    float m_fPositionX;
    float m_fPositionY;
    float m_fPositionZ;

    float m_fRotationX;
    float m_fRotationY;
    float m_fRotationZ;

    // Vertex array of our cube
    Vertex mVertices[8];
    unsigned int mIndices[36];
public:
    Cube();
    ~Cube();

    // Set up the cube
    void Init();

    // Get Address of the Vertex and Index arrays
    Vertex* GetVertexAddress();
    unsigned int* GetIndexAddress();

    // SET PARAMETERS //

    /*  Name: SetPosition
        Param: float, float, float
        Return: Void
        Info: Set x, y, and z position of cube
    */
    void SetPosition(float x, float y, float z);

    /*  Name: SetRotation
        Param: float, float, float
        Return: void
        Info: Set rotation of the x, y, and z axis
    */
    //void SetRotation(float x, float y, float z);

    /*  Name: SetTranslation
        Param: float, float, float
        Return void
        Info: Set the Translation of the x, y, and z axis
    */
    //void SetTranslation(float x, float y, float z);

    /*  Name: SetColor
        Param: float, float, float, float
        Return: void
        Info: Set the r, g, b, a of the color values. Range 0.0f - 1.0f.
    */
    void SetColor(float r, float g, float b, float a);

    // GET PARAMETERS //
    /*  Name: GetPositionX/Y/Z
        Param: void
        Return: float 
        Info: Get value of X/Y/Z
    */
    float GetPositionX();
    float GetPositionY();
    float GetPositionZ();

    /*  Name: GetRotationX/Y/Z
        Param: void
        Return: float
        Info: GetRotationX/Y/Z value
    */
    /*float GetRotationX();
    float GetRotationY();
    float GetRotationZ();*/

    /*  Name: GetSizeOfVerextStruct
        Param: void
        Return: int
        Info: Return the size of our Vertex struct
    */
    unsigned int GetSizeOfVertexStruct();

    /*  Name: GetSizeOfIndices
        Param: void
        Return: unsigned int
        Info: Return sizeof(array)/sizeof(array[0])
    */
    unsigned int GetSizeOfIndices();

    /*  Name: GetSizeOfVertices
        Param: void
        Return unsigned int
        Info: Return sizeof(array)/sizeof(array[0])
    */
    unsigned int GetSizeOfVertices();

    /*  Name: GetSizeOfVerticesArray()
        Param: void
        Return: unsigned int 
        Info: Return sizeof(array)
    */
    void** GetPointerToVerticesArray();

    /*  Name: GetSizeOfIndicesArray
        Param: void
        Return: unsigned int
        Info: Return sizeof(array)
    */
    void** GetPointerToIndicesArray();

    /*  Name: Render
        Param: float dt
        Return: void
        Info: Render our object
    */
    //void Render(float dt)

    /*
    */

};
#endif POLYGON_CUBE_H
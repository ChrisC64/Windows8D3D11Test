#pragma once

#include <DirectXMath.h>
#include <iostream>

class Vertex
{
protected:
    struct Position
    {
        float x;
        float y;
        float z;
    };

    struct Color
    {
        float x;
        float y;
        float z;
        float w;
    };

    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;

public:
    Vertex();
    ~Vertex();
};


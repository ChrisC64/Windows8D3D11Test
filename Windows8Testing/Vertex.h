#pragma once

#include <DirectXMath.h>
#include <iostream>
namespace Point
{
    class Vertex
    {
    protected:
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

            Position(float x, float y, float z)
            {
                this->x = x;
                this->y = y;
                this->z = z;
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

        struct Color
        {
            float r;
            float g;
            float b;
            float a;

            Color()
            {
                r = 1.0f;
                g = 1.0f;
                b = 1.0f;
                a = 0.0f;
            }

            Color(float r, float g, float b, float a)
            {
                this->r = r;
                this->g = g;
                this->b = b;
                this->a = a;
            }

            Color& operator=(const Color& c)
            {
                this->r = c.r;
                this->g = c.g;
                this->b = c.b;
                this->a = c.a;

                return *this;
            }
        };

        Position m_pos;
        Color m_color;

        //DirectX::XMFLOAT3 pos;
        //DirectX::XMFLOAT4 color;

    public:
        // Constructor
        Vertex();
        ~Vertex();
        // Override
        Vertex(float x, float y, float z, float r, float g, float b, float a);
        // Operator overrides
        Vertex& operator=(const Vertex& v);
        Vertex::Position& operator=(const Vertex::Position& p);
        Vertex::Color& operator=(const Vertex::Color& c);

        // Set Methods
        void SetColor(float r, float g, float b, float a);
        void SetColor(Color *color);
        void SetPosition(Position *pos);
        void SetPosition(float x, float y, float z);

        // Get Methods //
        Color* GetColor();
        Position* GetPosition();
    };
}


#include "Vertex.h"

using namespace Point;
Vertex::Vertex()
{
}

Vertex::~Vertex()
{
}

Vertex::Vertex(float x, float y, float z, float r, float g, float b, float a) :
m_pos(Position(x, y, z)), 
m_color(Color(r, g, b, a))
{
}

Vertex& Vertex::operator=(const Vertex& v)
{
   return *this = v;
}

Vertex::Position& Vertex::operator=(const Vertex::Position& p)
{
    this->m_pos.x = p.x;
    this->m_pos.y = p.y;
    this->m_pos.z = p.z;

    return this->m_pos;
}

Vertex::Color& Vertex::operator=(const Vertex::Color& c)
{
    this->m_color.r = c.r;
    this->m_color.g = c.g;
    this->m_color.b = c.b;
    this->m_color.a = c.a;

    return this->m_color;
}

// Set Methods
void Vertex::SetColor(float r, float g, float b, float a)
{
    m_color.r = r;
    m_color.g = g;
    m_color.b = b;
    m_color.a = a;
}

void Vertex::SetColor(Color *color)
{
    m_color = *color;
}

void Vertex::SetPosition(Position *pos)
{
    m_pos = *pos;
}

void Vertex::SetPosition(float x, float y, float z)
{
    m_pos.x = x;
    m_pos.y = y;
    m_pos.z = z;
}

Vertex::Color* Vertex::GetColor()
{
    return &m_color;
}

Vertex::Position* Vertex::GetPosition()
{
    return &m_pos;
}

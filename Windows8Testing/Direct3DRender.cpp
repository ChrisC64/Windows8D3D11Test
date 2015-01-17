#include "Direct3DRender.h"

using namespace Render;

Render::D3DRender::D3DRender()
{
}

Render::D3DRender::~D3DRender()
{
     Release();
}

void D3DRender::Release()
{
    SAFE_RELEASE(m_pBuffer);
    SAFE_RELEASE(m_pBackBuffer);
    //m_pAppDevice->Shutdown();
}

void D3DRender::Init(ID3D11Texture2D *pBuffer,
    ID3D11RenderTargetView *pBackBuffer, D3DApp *m_AppDevice)
{

}

void D3DRender::Render(float dt)
{
    if (dt > 0)
        return;

}

void D3DRender::Update(float dt)
{

}


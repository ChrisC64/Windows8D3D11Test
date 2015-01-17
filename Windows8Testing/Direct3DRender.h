/*********************************
** This will be my header file for rendering
** the game. It is soley designed to take objects,
** draw them on the screen, handle all calls to rendering objects.
** I will start off by a blank screen, then hopefully later design it
** more abstract instead, so that it is just inherited by other classes,
** specifically the World class
**/
//
#pragma once
#include "pch.h"
#include "Direct3DApp.h"
#include "GameObject.h"
#include "GameObject_Cube.h"
#define SAFE_RELEASE(x) if(x){x->Release(); x = nullptr;}
namespace Render 
{
    class D3DRender : public D3DApp
    {
    protected:
        ID3D11Texture2D* m_pBuffer;
        ID3D11RenderTargetView* m_pBackBuffer;

        D3DApp*     m_pAppDevice;
    public:
        D3DRender();
        ~D3DRender();
        
        void Init(ID3D11Texture2D *pBuffer, 
            ID3D11RenderTargetView *pBackBuffer, D3DApp *m_AppDevice);

        void Render(float dt);
        void Update(float dt);
        void Release();
    };
}

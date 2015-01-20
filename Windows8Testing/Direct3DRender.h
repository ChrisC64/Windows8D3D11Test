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
#include "Polygon_Cube.h"
#include <vector>
#define SAFE_RELEASE(x) if(x){x->Release(); x = nullptr;}
namespace Render 
{
    class D3DRender : public D3DApp
    {
    private:
        struct ConstantBuffer
        {
            DirectX::XMMATRIX world;
            DirectX::XMMATRIX view;
            DirectX::XMMATRIX proj;
        };

        // Temp Use for practice
        DirectX::XMMATRIX       m_View;
        DirectX::XMMATRIX       m_World;
        DirectX::XMMATRIX       m_Projection;
        DirectX::XMMATRIX       m_ScaleMat;
        DirectX::XMMATRIX       m_TranslationMat;
        DirectX::XMMATRIX       m_RotationMat;

        Cube                    mPolyCube;
        Cube                    mDynCube;
        std::vector<Cube*>      mVecCubes;
        
        float                   mFps;
    protected:
        // Pointers to objects created by the Device in D3DApp class //
        // Over time we'll see which ones can be initiated and created from this class //
        ID3D11Texture2D*        m_pBuffer;
        ID3D11RenderTargetView* m_pBackBuffer;
        IDXGISwapChain*         m_pSwapChain;
        ID3D11DeviceContext*    m_pDeviceContext;
        ID3D11Device*           m_pD3dDevice;
        ID3D11RenderTargetView* m_pRenderTargetView;
        ID3D11DepthStencilView* m_pDepthStencilView;
        //DepthStencilBuffer, IndexBuffer, VertexBuffer might need to be created
        // Buffer //
        ID3D11Buffer*           m_pConstantBuffer;
        ID3D11Buffer*           m_pVertexBuffer;
        ID3D11Buffer*           m_pIndexBuffer;

        // Pointer to the class object of our device / immediate context
        D3DApp*                 m_pAppDevice;

        // Handler to Window
        HWND*                   m_pHwnd;
        // Used to create color for viewport
        float                   color[4];

        // Shaders
        ID3D11PixelShader*      m_pPixelShader;
        ID3D11VertexShader*     m_pVertexShader;

        // Blobs
        ID3DBlob*               m_pErrorMsgBlob;

    public:
        D3DRender();
        ~D3DRender();
        
        bool Init(D3DApp *m_AppDevice);
        void RenderBuffer();
        void Render(float dt, std::vector<Object::GameObject*> *pObjects);
        void Update(float dt);
        void Release();

        HRESULT InitConstantBuffer();
        HRESULT InitRenderTarget();
        HRESULT InitVertBuffer(/*UINT ByteWidth, void** dataLocation, UINT stride, UINT offset */);
        HRESULT AddToVertBuffer(UINT ByteWidth, void** dataLocation, UINT stride, UINT offset);
        HRESULT InitIndexBuffer(/*UINT size, void** dataLocation*/);
        HRESULT AddToIndexBuffer(UINT size, void** dataLocation);
        HRESULT CompileShaders();
        //HRESULT SetSwapChainDesc();
        HRESULT InitDepthStencilDesc();
        //HRESULT CreateSwapChain(); // Consider creating the swap chain separate from the device

        // Helper function for compiling our shader files
        HRESULT CompileFile(WCHAR* fileName, LPCSTR entryPoint,
            LPCSTR shaderModel, ID3DBlob** ppBlobOut);

        void SetViewPort(int width, int height);
        void SetRasterState();
        void InitWorldMatrix();

        void CalcFrames(float dt);
    };
}

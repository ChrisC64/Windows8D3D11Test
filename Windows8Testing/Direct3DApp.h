/***************************
** In this application, we'll be doing some basic functions for
** setting up a Direct3D environment. These functions are:
**  Creating and Registering the device, Reading keyboard input,
**  and Managing issues like losing the devices
***/
#pragma once
#include "pch.h"
#include "Polygon_Cube.h"
#include <string.h>

// Add libraries //
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "Dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define ASSERT(x) {(x > 0) ? true : false;}
#define SAFE_RELEASE(x) if(x){x->Release(); x = nullptr;}
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

//extern ID3D11Device* g_D3dDevice;
//extern ID3D11DeviceContext* g_D3dDevCon;
//extern IDXGISwapChain* g_SwapChain;


class D3DApp
{
private:
    Cube mCube;
private:
    // Window Variables
    HWND                    mHWnd;          // Handler to the window
    bool                    mbVsync;        // Boolean for vertical syncing

    // Timer Variables - NOT USED
    float                   mCurrTime;      // Current time; used in timer
    float                   mPrevTime;      // Previous time
    int                     mFps;           // Frames per second counter
    float                   mMillSecFps;    // Millisecond frames per second counter

    // Timer Variables - NOT USED
    __int64                 mCntsPerSec;    // CPU counters per second
    __int64                 mPrevTimeStamp; // Previous time stamp 
    __int64                 mCurrTimeStamp; // Current time stamp
    float                   mDeltaTime;      // Delta Time (CurrTimeStamp - PrevTimeStamp) * secsPerCnt
    float                   mSecsPerCnt;    // Seconds per CPU frequency (1.0f / cntsPerSec)

    // Direct3D Variables 
    ID3D11Device*           mpD3dDevice;    // Pointer to the D3D Device
    ID3D11DeviceContext*    mpD3dImmediateContext;// Pointer to the D3D Device Context; Device Context manages the GPU Pipeline for rendering graphics and determining how they will be rendered
    IDXGISwapChain*         mpSwapChain;    // Pointer to the Swap Chain buffer

    // Direct3D Desc
    DXGI_SWAP_CHAIN_DESC    mScd;           // Swap Chain struct
    D3D_FEATURE_LEVEL       mFeatureLevel;  // Feature level support

    // Video buffers
    ID3D11Texture2D*        mpBackBuffer;
    ID3D11RenderTargetView* mpRenderTargetView;

    // Textures
    ID3D11Texture2D*        mpDepthStencilBuffer;
    ID3D11DepthStencilView* mpDepthStencilView;

    // BUffers
    ID3D11Buffer*           mVertexBuffer;
    ID3D11Buffer*           mPosColorBuffer;
    ID3D11Buffer*           mIndexBuffer;
    ID3D11Buffer*           mConstantBuffer;

    // Inpute Layout
    ID3D11InputLayout*       mInputLayout;

    // Compiler Items
    ID3DBlob*               mpVSBlob;
    ID3DBlob*               mpPSBlob;
    ID3DBlob*               mpErrorMsgBlob;
    ID3D11PixelShader*      mpPixelShader;
    ID3D11VertexShader*     mpVertexShader;

    // Color
    float                   color[4];

    // Booleans
    bool                    mbIsPaused;       // Pause application

    // Temp Use for practice
    DirectX::XMMATRIX m_View;
    DirectX::XMMATRIX m_World;
    DirectX::XMMATRIX m_Projection;
    DirectX::XMMATRIX m_ScaleMat;
    DirectX::XMMATRIX m_TranslationMat;
    DirectX::XMMATRIX m_RotationMat;
    // Struct for creating a triangle to display on screen

    // POS and COLOR struct

    struct VertexPC
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT4 color;
    };

    struct Vertex2
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 Tex0;
        DirectX::XMFLOAT2 Tex1;
    };

    struct ConstantBuffer
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX proj;
    };

    // variables
    //VertexPC    vertices[4];

public:

    // Class Constructor
    D3DApp();
    ~D3DApp();

    /////////////////
    // GET METHODS //
    /////////////////
    DirectX::XMMATRIX& GetWorldMat();
    DirectX::XMMATRIX& GetViewMat();
    DirectX::XMMATRIX& GetProjMat();
    
    ID3D11DeviceContext* GetDeviceContext();
    ID3D11RenderTargetView* GetRenderTargetView();
    ID3D11Device* GetDevice();
    IDXGISwapChain* GetSwapChain();
    HWND* GetHWND();

    // SET METHODS //
    void SetWorldMat(DirectX::XMMATRIX &worldMat);
    void SetViewMat(DirectX::XMMATRIX &viewMat);
    void SetProjMat(DirectX::XMMATRIX &projMat);

    /* Name: InitDevice(HWND hwnd)
       Params: Hwnd (Handler to window)
       Return: HRESULT
       Info: This function is used to initalize and set up our D3D Device
       */
    bool InitDevice(HWND hWnd);

    /* Name: InitRenderTarget()
       Param: Void
       Return: HRESULT
       Info: To keep things clean, initialize the buffer and scene in this function
       while being called within the class only.
       */
    HRESULT InitRenderTarget();

    /* Name: SetViewPort(int width, int height)
       Param: Int width, Int Height
       Return: void
       Info: Set the viewport for display. Take in Width and Height in pixels
       */
    void SetViewPort(int width, int height);

    /* Name: Render(float dt)
        Param: float deltaTime
        Return: void
        Info: Render a frame
        */
    void Render(float dt);

    /*  Name: InitDepthStencilDesc()
        Param: Void
        Retrn: HRESULT
        Info: Init and set up the Depth Stencil buffer device
        */
    HRESULT InitDepthStencilDesc();

    /*  Name: CompileFile
        Param: WCHAR*, LPCSTR, LPCSTR, ID3DBlob**
        Return: HRESULT
        Info: Enable us to compile the necessary shader files in a contained function
        versus making the various calls in other places of the code.
        */
    HRESULT CompileFile(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** ppBlobOut);

    /*  Name: CompileShaders
        Param: void
        Return: bool
        Info: Here we'll compile the shader files we have currently.
        */
    HRESULT CompileShaders();

    /*  Name: InitSwapChain()
        Param: Void
        Info: Set up the Swap Chain Description in this function. Helps keep code a little neater.
        */
    HRESULT SetSwapChainDesc();

    /*  Name: CalcFps()
        Param: Void
        Info: Calculate the Frames per second
        */
    void CalcFps();

    /*  Name: InitVertBuffer
        Param: Void
        Info: Initialize D3DApp's vertex buffer for displaying our 3D Object
        */
    HRESULT InitVertBuffer();

    /*  Name: InitIndexBuffer
        Param: void
        Info: Initialize our D3DApp's index buffer
        */
    HRESULT InitIndexBuffer();

    /*  Name: InitWorldMatrix
        Param: void
        Info: This is going to set our Matrices used for our world, lookAt, Eye, Up, view, and Projection.
        */
    void InitWorldMatrix();

    /*  Name: InitConstantBuffer
        Param: void
        Info: Initialize the constant buffer that will be used for our world, view, and proj matrices.
        */
    HRESULT InitConstantBuffer();

    /*  Name: SetRasterizerState()
        Param: void
        Info: Set the Rasterizer description for the device.
        */
    void SetRasterState();

    /* Name: Shutdown(void)
       Paras: void
       Return: void
       Info: Release all COM objects and free up memory in use.
       */
    void Shutdown(void);


};
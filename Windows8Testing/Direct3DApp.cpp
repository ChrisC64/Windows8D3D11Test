#include "Direct3DApp.h"

D3DApp::D3DApp() : mHWnd(0),
    mbVsync(false),
    mCurrTime(0.0),
    mPrevTime(0.0),
    mFps(0),
    mMillSecFps(0.0),
    mCntsPerSec(0),
    mPrevTimeStamp(0),
    mCurrTimeStamp(0),
    mDeltaTime(0),
    mSecsPerCnt(0),
    mpD3dDevice(NULL),
    mpD3dDevContext(0),
    mpSwapChain(0),
    mpBackBuffer(0),
    mpRenderTargetView(0),
    mVertexBuffer(0),
    mInputLayout(0),
    mPosColorBuffer(0)
{
    color[0] = 0.0f;
    color[1] = 0.2f;
    color[2] = 0.4f;
    color[3] = 1.0f;
}

D3DApp::~D3DApp()
{
    Shutdown();
}


// We initialized the D3D Device 
HRESULT D3DApp::InitDevice(HWND hWnd)
{
    mHWnd = hWnd;

    SetSwapChainDesc();

    HRESULT hr;

    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
    creationFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

    // Set up Feature Level Desc array
    D3D_FEATURE_LEVEL featureLevels[]
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    // Create the device now
    hr = D3D11CreateDeviceAndSwapChain(
        NULL,                                               // IDXGI Adapter
        D3D_DRIVER_TYPE_HARDWARE,                           // D3D Driver type: Use hardware
        NULL,                                               // HMODULE: handle to DLL software rasterizer
        creationFlags,                                      // D3D11_CREATE_DEVICE_FLAG
        featureLevels,                                      // D3D_FEATURE_LEVEL flags
        ARRAYSIZE(featureLevels),                           // UINT Feature levels
        D3D11_SDK_VERSION,                                  // SDK Version
        &mScd,                                              // IDGXI_SWAP_CHAIN_DESC address
        &mpSwapChain,                                       // Swap chain address
        &mpD3dDevice,                                       // pointer to the device
        &mFeatureLevel,                                     // pointer to feature level support: D3D_FEATURE_LEVEL
        &mpD3dDevContext                                    // pointer to the device context
        );

    if (FAILED(hr))
    {
        printf_s("Failed to create device. Error: ");
        MessageBox(mHWnd, L"Failed to Create Device", L"Device Creation Failed", MB_ICONERROR);
        throw(hr);
    }

    hr = InitRenderTarget();

    if (FAILED(hr))
    {
        printf_s("Failed to init render target.");
        MessageBox(mHWnd, L"Failed to Init Render Target", L"Render Target Failed", MB_ICONERROR);
        throw(hr);
    }

    SetViewPort(SCREEN_WIDTH, SCREEN_HEIGHT);

    return(hr);
}

HRESULT D3DApp::InitRenderTarget()
{
    if (!mpD3dDevice)
        return(0);
    
    HRESULT hr;

    // Get buffer address for the swap chain
    hr = mpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&mpBackBuffer);

    if (FAILED(hr))
    {
        printf_s("Failed to obtain back buffer address from swap chain");
        return(hr);
    }
    
    // Use buffer address now to create the render target to the device
    hr = mpD3dDevice->CreateRenderTargetView(mpBackBuffer, NULL, &mpRenderTargetView);
    mpBackBuffer->Release();

    if (FAILED(hr))
    {
        printf_s("Failed to create Render Target View");
        return(hr);
    }

    hr = InitDepthStencilDesc();
    if (FAILED(hr))
    {
        printf_s("Failed to init depth stencil");
        return(hr);
    }

    // Set the Render Target
    mpD3dDevContext->OMSetRenderTargets(1, &mpRenderTargetView, NULL);

    return(hr);
}

HRESULT D3DApp::InitDepthStencilDesc()
{
    D3D11_TEXTURE2D_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(D3D11_TEXTURE2D_DESC));

    HRESULT hr;
    // Set parameters
    dsDesc.Width = SCREEN_WIDTH;
    dsDesc.Height = SCREEN_HEIGHT;
    dsDesc.MipLevels = 1;
    dsDesc.ArraySize = 1;
    dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // Set MSAA values equal to swap chain 
    dsDesc.SampleDesc.Count = 4;

    dsDesc.Usage = D3D11_USAGE_DEFAULT;
    dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    dsDesc.CPUAccessFlags = 0;
    dsDesc.MiscFlags = 0;

    //ZeroMemory(&mpDepthStencilBuffer, sizeof(ID3D11Texture2D));

    if (FAILED(hr = mpD3dDevice->CreateTexture2D(&dsDesc, 0, &mpDepthStencilBuffer)))
    {
        MessageBox(mHWnd, L"Failed to create Depth Stencil Buffer", L"ERROR: Depth Stencil Buffer", MB_ICONERROR);
        return(hr);
    }

    if (FAILED(hr = mpD3dDevice->CreateDepthStencilView(mpDepthStencilBuffer, 0, &mpDepthStencilView)))
    {
        MessageBox(mHWnd, L"Failed to create Depth Stencil View", L"ERROR: Depth Stencil View", MB_ICONERROR);
        return(hr);
    }

    return(hr);
}

void D3DApp::SetViewPort(int width, int height)
{
    if (!mpD3dDevice)
        return;

    D3D11_VIEWPORT  viewport;

    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    // Set up parameters
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = width;
    viewport.Height = height;

    mpD3dDevContext->RSSetViewports(2, &viewport);
}

void D3DApp::SetSwapChainDesc()
{
    // TODO: Either modify this or create separate function to change the Swap Chain Desc's settings such as MSAA Sample
    
    // Zero memory the structure
    ZeroMemory(&mScd, sizeof(DXGI_SWAP_CHAIN_DESC));

    HRESULT hr;
    
    if (mpD3dDevice)
    {
        UINT msaaQuality;
        hr = mpD3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaaQuality);
    }

    // Set the swap chain structure
    mScd.BufferCount = 1;                             // Size of back buffer
    mScd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // Set the display buffer format (32 bit format)
    mScd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // Use the surface or resource as an output render only
    mScd.OutputWindow = mHWnd;                              // Handler to the window that will be used
    mScd.SampleDesc.Count = 4;                              // How many multi samples to use
    mScd.Windowed = true;                                   // Window mode
    mScd.BufferDesc.Width = SCREEN_WIDTH;                   // Back buffer width
    mScd.BufferDesc.Height = SCREEN_HEIGHT;                 // Backk buffer height
    mScd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // Allow full-screen switching
}

void D3DApp::CalcFps()
{

}

void D3DApp::Render(float dt)
{
    // TODO: Consider making this a virtual function (?)
    if (!mpD3dDevice)
    {
        return;
    }

    //// Set up of Floats to load into our matrix
    //DirectX::XMFLOAT4X4 pos(/*m11*/0.0f, 0.0f, 0.0f, 0.0f, 
    //    /*m21*/0.0f, 5.0f, 0.0f, 0.0f, 
    //    /*m31*/0.0f, 0.0f, -10.0f, 0.0f,
    //    /*m41*/0.0f, 0.0f, 0.0f, 0.0f);
    //DirectX::XMFLOAT4X4 lookAt(/*m11*/0.0f, 0.0f, 0.0f, 0.0f, 
    //    /*m21*/0.0f, 0.0f, 0.0f, 0.0f,
    //    /*m31*/ 0.0f, 0.0f, 0.0f, 0.0f, 
    //    /*m41*/0.0f, 0.0f, 0.0f, 0.0f);
    //DirectX::XMFLOAT4X4 up(/*m11*/0.0f, 0.0f, 0.0f, 0.0f,
    //    /*m21*/0.0f, 1.0f, 0.0f, 0.0f,
    //    /*m31*/ 0.0f, 0.0f, 0.0f, 0.0f, 
    //    /*m41*/0.0f, 0.0f, 0.0f, 0.0f);

    //// Load XMFLOATs into XMMAtrix
    //DirectX::XMMATRIX matPos = DirectX::XMLoadFloat4x4(&pos);
    //DirectX::XMMATRIX matLookAt = DirectX::XMLoadFloat4x4(&lookAt);
    //DirectX::XMMATRIX matUp = DirectX::XMLoadFloat4x4(&up);

    //DirectX::XMMATRIX matResult = DirectX::XMMatrixIdentity();

    //matResult = DirectX::XMMatrixMultiply(matResult, matPos);
    //matResult = DirectX::XMMatrixMultiply(matResult, matLookAt);
    //matResult = DirectX::XMMatrixMultiply(matResult, matUp);
    
    // Set View Matrix Identity
    m_View = DirectX::XMMatrixIdentity();


    // Create XMFloat3s as eyePos, LookAt, and Up
    DirectX::XMFLOAT3 eyePos;
    DirectX::XMFLOAT3 lookAt;
    DirectX::XMFLOAT3 up;

    eyePos.x = 0.0f;
    eyePos.y = 5.0f;
    eyePos.z = -10.0f;

    lookAt.x = 0.0f;
    lookAt.y = 0.0f;
    lookAt.z = 0.0f;

    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    // Set up world view matrix
    //DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
    //DirectX::XMStoreFloat4x4(&mView, matResult);

    m_View = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&eyePos), DirectX::XMLoadFloat3(&lookAt), DirectX::XMLoadFloat3(&up));

    // Clear the back buffer to a color
    mpD3dDevContext->ClearRenderTargetView(mpRenderTargetView, color);
    // Clear depth stencil view
    mpD3dDevContext->ClearDepthStencilView(mpDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    
    // Set Input Layout
    mpD3dDevContext->IASetInputLayout(mInputLayout);
    mpD3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(VertexPC);
    UINT offset = 0;
    // Set Vertex Buffers
    mpD3dDevContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

    mpD3dDevContext->DrawIndexed(36, 0, 0);

    // Render any other parts of the scene here (implement later: the world matrix, rot matrix, etc)
    /*if (dt >= 1.0f)
    {
        color[0] += 0.001f;
        color[1] += 0.001f;
        color[2] += 0.001f;
    }*/

    SetWindowText(mHWnd, L"Chris Carlos: D3D11 Demo");
    
    // Present the scene
    mpSwapChain->Present(0,0);

    return;

}

void D3DApp::Shutdown(void)
{
    mpSwapChain->SetFullscreenState(false, NULL);

    SAFE_RELEASE(mpD3dDevContext);
    SAFE_RELEASE(mpD3dDevContext);
    SAFE_RELEASE(mpSwapChain);
    SAFE_RELEASE(mpRenderTargetView);
    SAFE_RELEASE(mVertexBuffer);
    SAFE_RELEASE(mInputLayout);
}

void D3DApp::SetCube()
{
    // Colors
    DirectX::XMFLOAT4 White = { 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };

    // Assigning vertices pos and color with two lines : Just because...
    vertices[0].pos = DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f);
    vertices[0].color = White;
    // Assign vertices pos and color in one line
    vertices[1] = { DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), Black };
    vertices[2] = { DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), Red };
    vertices[3] = { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), Green };
    vertices[4] = { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), Blue };
    vertices[5] = { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), Yellow };
    vertices[6] = { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), Cyan };
    vertices[7] = { DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), Magenta };

    // Set up buffer description
    D3D11_BUFFER_DESC vbd;

    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(VertexPC) * 8;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    // Set up subsource data: This describes the data we want to initialize the buffer contents with
    D3D11_SUBRESOURCE_DATA vInitData;
    
    vInitData.pSysMem = vertices;

    HRESULT hr;

    hr = mpD3dDevice->CreateBuffer(
        &vbd,
        &vInitData,
        &mVertexBuffer);

    if (FAILED(hr))
    {
        MessageBox(mHWnd, L"Failed to create Cube Buffer!", L"Buffer Creat Failed", MB_ICONERROR);
        return;
    }

    // Set up Index Buffer of CUBE

    UINT indices[] =
    {
        // FRONT FACE
        0, 1, 2,
        0, 2, 3,

        // BACK FACE
        4, 6, 5,
        4, 7, 6,

        // LEFT FACE
        4, 5, 1,
        4, 1, 0,

        // RIGHT FACE
        3, 2, 6,
        3, 6, 7,

        // TOP FACE
        1, 5, 6,
        1, 6, 2,

        // BOTTOM FACE
        4, 0, 3,
        4, 3, 7
    };

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * 36;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iInitData;
    iInitData.pSysMem = indices;
    
    hr = mpD3dDevice->CreateBuffer(&ibd, &iInitData, &mIndexBuffer);

    if (FAILED(hr))
    {
        MessageBox(mHWnd, L"Failed to create Cube Buffer!", L"INDEX BUFFER Create Failed", MB_ICONERROR);
        return;
    }

    // Set Vertex Input Layout
    //D3D11_INPUT_ELEMENT_DESC vertDesc[] =
    //{
    //    { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    //    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    //};

    //hr = mpD3dDevice->CreateInputLayout(vertDesc, 2, 0, 0, &mInputLayout);

    //if (FAILED(hr))
    //{
    //    MessageBox(mHWnd, L"Failed to create Input Layout", L"Input Layout Create Failed", MB_ICONERROR);
    //    return;
    //}

    return;
}
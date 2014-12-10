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
    mPosColorBuffer(0),
    mConstantBuffer(nullptr)
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

    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags = D3D11_CREATE_DEVICE_DEBUG;
#elif 
    creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_SOFTWARE

    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    // Set up Feature Level Desc array
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
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
        MessageBox(mHWnd, L"Failed to Create Device and SwapChain", L"Device Creation Failed", MB_ICONERROR);
        throw(hr);
    }

    //// Obtain DXGI Factorysince we used NULL for above
    //IDXGIFactory1* dxgiFactory = nullptr;
    //IDXGIDevice* dxgiDevice = nullptr;

    //hr = mpD3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    //if (SUCCEEDED(hr))
    //{
    //    // Get Adapter - Display Sub-system - of the device (graphics card or motherboard, may vary on device)
    //    IDXGIAdapter* adapter = nullptr;
    //    hr = dxgiDevice->GetAdapter(&adapter);
    //    if (SUCCEEDED(hr))
    //    {
    //        hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
    //        adapter->Release();
    //    }
    //    dxgiDevice->Release();
    //}

    //if (FAILED(hr))
    //{
    //    MessageBox(mHWnd, L"Failed to QueryInterface of the devie", L"QueryInteraceFailed", MB_OK);
    //}

    hr = InitRenderTarget();

    if (FAILED(hr))
    {
        printf_s("Failed to init render target.");
        MessageBox(mHWnd, L"Failed to Init Render Target", L"Render Target Failed", MB_ICONERROR);
        throw(hr);
    }

    SetViewPort(SCREEN_WIDTH, SCREEN_HEIGHT);

    SetRasterState();

    InitWorldMatrix();

    InitConstantBuffer();

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
    viewport.MaxDepth = 1.0f;

    mpD3dDevContext->RSSetViewports(1, &viewport);
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
    mScd.BufferCount = 1;                                   // Size of back buffer
    mScd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // Set the display buffer format (32 bit format)
    mScd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // Use the surface or resource as an output render only
    mScd.OutputWindow = mHWnd;                              // Handler to the window that will be used
    mScd.SampleDesc.Count = 4;                              // How many multi samples to use
    mScd.SampleDesc.Quality = 0;
    mScd.Windowed = true;                                   // Window mode
    mScd.BufferDesc.Width = SCREEN_WIDTH;                   // Back buffer width
    mScd.BufferDesc.Height = SCREEN_HEIGHT;                 // Backk buffer height
    mScd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // Allow full-screen switching
    mScd.OutputWindow = mHWnd;                              // Handler to output window
    mScd.Windowed = TRUE;                                   // Window or FullScreen
    mScd.BufferDesc.RefreshRate.Numerator = 60;             // Refresh Rate top
    mScd.BufferDesc.RefreshRate.Denominator = 1;            // Refresh Rate Bottom 60 / 1 = 60FPS

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
    // Update our time
    static float t = 0.0f;
   
   static ULONGLONG timeStart = 0;
   ULONGLONG timeCur = GetTickCount64();
   if (timeStart == 0)
       timeStart = timeCur;
   t = (timeCur - timeStart) / 1000.0f;
   

    //
    // Animate the cube
    //
    m_World = DirectX::XMMatrixRotationRollPitchYaw (t / 4, t/2, t * 2);
    //m_World = DirectX::XMMatrixRotationY(t / 2);

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
    
    // Set up world view matrix
    //DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
    //DirectX::XMStoreFloat4x4(&mView, matResult);

    // Clear the back buffer to a color
    mpD3dDevContext->ClearRenderTargetView(mpRenderTargetView, color);

    //m_World = DirectX::XMMatrixIdentity();

    // Transpose our Matrices
    ConstantBuffer cb;

    cb.world = DirectX::XMMatrixTranspose(m_World);
    cb.view = DirectX::XMMatrixTranspose(m_View);
    cb.proj = DirectX::XMMatrixTranspose(m_Projection);
    mpD3dDevContext->UpdateSubresource(mConstantBuffer, 0, nullptr, &cb, 0, 0);

    // Clear depth stencil view
    //mpD3dDevContext->ClearDepthStencilView(mpDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    
    // Set Vertex Shader
    mpD3dDevContext->VSSetShader(mpVertexShader, nullptr, 0);
    // Set Constant buffer
    mpD3dDevContext->VSSetConstantBuffers(0, 1, &mConstantBuffer);
    // Set Pixel Shader
    mpD3dDevContext->PSSetShader(mpPixelShader, nullptr, 0);


    mpD3dDevContext->DrawIndexed(36, 0, 0);

    SetWindowText(mHWnd, L"Chris Carlos: D3D11 Demo");
    
    // Present the scene
    mpSwapChain->Present(0,0);

    return;

}

void D3DApp::Shutdown(void)
{
    mpSwapChain->SetFullscreenState(false, NULL);
    mpD3dDevContext->ClearState();
    SAFE_RELEASE(mpD3dDevContext);
    SAFE_RELEASE(mpD3dDevice);
    SAFE_RELEASE(mpSwapChain);
    SAFE_RELEASE(mpBackBuffer);
    SAFE_RELEASE(mpRenderTargetView);
    SAFE_RELEASE(mpDepthStencilBuffer);
    SAFE_RELEASE(mpDepthStencilView);
    SAFE_RELEASE(mVertexBuffer);
    SAFE_RELEASE(mInputLayout);
    SAFE_RELEASE(mpVSBlob);
    SAFE_RELEASE(mpPSBlob);
    SAFE_RELEASE(mpErrorMsgBlob);
    SAFE_RELEASE(mpPixelShader);
    SAFE_RELEASE(mpVertexShader);
    SAFE_RELEASE(mIndexBuffer);
    SAFE_RELEASE(mVertexBuffer);
    SAFE_RELEASE(mPosColorBuffer);
    SAFE_RELEASE(mConstantBuffer);

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

    VertexPC vertices[] =
    {
        // Assign vertices pos and color in one line
        { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), White },
        { DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), Black },
        { DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), Red },
        { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), Green },
        { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), Blue },
        { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), Yellow },
        { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), Cyan },
        { DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), Magenta }
    };


    // Create vertex buffer
    //VertexPC vertices[] =
    //{
    //    { DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
    //    { DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
    //    { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
    //    { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
    //    { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
    //    { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
    //    { DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
    //    { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
    //};
    HRESULT hr;

    // Set up buffer description
    D3D11_BUFFER_DESC vbd;
    ZeroMemory(&vbd, sizeof(vbd));
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(VertexPC) * (sizeof(vertices)/sizeof(vertices[0]));
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    // Set up subsource data: This describes the data we want to initialize the buffer contents with
    D3D11_SUBRESOURCE_DATA vInitData;
    ZeroMemory(&vInitData, sizeof(vInitData));
    vInitData.pSysMem = vertices;


    hr = mpD3dDevice->CreateBuffer(
        &vbd,
        &vInitData,
        &mVertexBuffer);

    if (FAILED(hr))
    {
        MessageBox(mHWnd, L"Failed to create Cube Buffer!", L"Buffer Creat Failed", MB_ICONERROR);
        return;
    }

    UINT stride = sizeof(VertexPC);
    UINT offset = 0;

    // Set Vertex Buffers
    mpD3dDevContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

    // Set up Index Buffer of CUBE
    // IF DXGI_FORMAT_R32_UINT, use UINT for Indices[] data type, else set to WORD if DXGI_FORMAT_R16_UINT (let's see if we don't even need to change that with some tests!)

    UINT indices[] =
    {
        // FRONT FACE
        2, 0, 1,
        0, 2, 3,

        // BACK FACE
        6, 5, 4,
        4, 7, 6,

        // LEFT FACE
        1, 0, 5,
        0, 4, 5,

        // RIGHT FACE
        2, 6, 3,
        7, 3, 6,

        // TOP FACE
        6, 2, 5,
        1, 5, 2,

        // BOTTOM FACE
        7, 4, 0,
        3, 7, 0,

        /*3, 1, 0,
        2, 1, 3,

        0, 5, 4,
        1, 5, 0,

        3, 4, 7,
        0, 4, 3,

        1, 6, 5,
        2, 6, 1,

        2, 7, 6,
        3, 7, 2,*/

        /*6, 4, 5,
        7, 4, 6,*/
    };
    D3D11_BUFFER_DESC ibd;
    ZeroMemory(&ibd, sizeof(ibd));
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(UINT) * (sizeof(indices)/sizeof(indices[0]));
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

    // Set Index Buffer 
    mpD3dDevContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set Primitive Topology
    mpD3dDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

     //Set Vertex Input Layout
    D3D11_INPUT_ELEMENT_DESC vertDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    UINT numElements = ARRAYSIZE(vertDesc);

    // Compile shaders before creating input layout
    // Vertex Shader
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG;
    flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    LPCSTR profile = "vs_5_0";

    hr = D3DCompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, "VS", profile, flags, 0, &mpVSBlob, &mpErrorMsgBlob);

    if (FAILED(hr))
    {
        if (mpErrorMsgBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(mpErrorMsgBlob->GetBufferPointer()));
            mpErrorMsgBlob->Release();
        }

        if (mpVSBlob)
        {
            mpVSBlob->Release();
        }
    }

    // Create the Vertex Shader
    hr = mpD3dDevice->CreateVertexShader(mpVSBlob->GetBufferPointer(), mpVSBlob->GetBufferSize(), nullptr, &mpVertexShader);

    if (FAILED(hr))
    {
        MessageBox(mHWnd, L"Failed to create vertex shader", L"Vertex Shader Creation Failed!", MB_ICONERROR);
        mpVSBlob->Release();
    }

    hr = mpD3dDevice->CreateInputLayout(vertDesc, numElements/*size of vertDesc*/, mpVSBlob->GetBufferPointer(), mpVSBlob->GetBufferSize(), &mInputLayout);

    if (FAILED(hr))
    {
        MessageBox(mHWnd, L"Failed to create Input Layout", L"Input Layout Create Failed", MB_ICONERROR);
        return;
    }

    // Set Input Layout
    mpD3dDevContext->IASetInputLayout(mInputLayout);

    // Set up Pixel Shader
    profile = "ps_5_0";
    hr = D3DCompileFromFile(L"PixelShader.hlsl", nullptr, nullptr/*D3D_COMPILE_STANDARD_FILE_INCLUDE*/, "PS", profile, flags, 0, &mpPSBlob, &mpErrorMsgBlob);

    if (FAILED(hr))
    {
        if (mpErrorMsgBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(mpErrorMsgBlob->GetBufferPointer()));
            mpErrorMsgBlob->Release();
        }

        if (mpPSBlob)
        {
            mpPSBlob->Release();
        }
    }

    hr = mpD3dDevice->CreatePixelShader(mpPSBlob->GetBufferPointer(), mpPSBlob->GetBufferSize(), nullptr, &mpPixelShader);

    if (FAILED(hr))
    {
        MessageBox(mHWnd, L"Failed to create pixel shader", L"Pixel shader creation failed!", MB_ICONERROR);
        mpPSBlob->Release();
    }

    return;
}

void D3DApp::InitWorldMatrix()
{
    // World Matrix Init
    m_World = DirectX::XMMatrixIdentity();
    m_View = DirectX::XMMatrixIdentity();
    m_Projection = DirectX::XMMatrixIdentity();

    // Rect struct
    RECT rc;
    GetClientRect(mHWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
    DirectX::XMVECTOR lookAt = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    // Set Camera Position and View
    //eyePos.x = 0.0f;
    //eyePos.y = 1.0f;
    //eyePos.z = -10.0f;

    //lookAt.x = 0.0f;
    //lookAt.y = 1.0f;
    //lookAt.z = 0.0f;

    //up.x = 0.0f;
    //up.y = 1.0f;
    //up.z = 0.0f;

    // Set View matrix up
    //m_View = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&eyePos), DirectX::XMLoadFloat3(&lookAt), DirectX::XMLoadFloat3(&up));
    m_View = DirectX::XMMatrixLookAtLH(eyePos, lookAt, up);
    m_Projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, width / (float)height, 0.01f, 1000.0f);
}

void D3DApp::InitConstantBuffer()
{
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage        = D3D11_USAGE_DEFAULT;
    bd.ByteWidth    = sizeof(ConstantBuffer);
    bd.BindFlags    = D3D10_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;

    HRESULT hr;

    hr = mpD3dDevice->CreateBuffer(&bd, nullptr, &mConstantBuffer);

    if (FAILED(hr))
    {
        MessageBox(mHWnd, L"Failed to create Constant Buffer!", L"Constant Buffer Init Failed!", MB_OK);
    }
}

void D3DApp::SetRasterState()
{
    if (!mpD3dDevContext)
    {
        return;
    }

    ID3D11RasterizerState* pRaster;
    
    D3D11_RASTERIZER_DESC noCullingDesc;
    ZeroMemory(&noCullingDesc, sizeof(D3D11_RASTERIZER_DESC));

    noCullingDesc.FillMode = D3D11_FILL_SOLID;
    noCullingDesc.CullMode = D3D11_CULL_BACK;
    noCullingDesc.FrontCounterClockwise = false;
    noCullingDesc.DepthBias = 0;
    noCullingDesc.SlopeScaledDepthBias = 0.0f;
    noCullingDesc.DepthBiasClamp = 0.0f;
    noCullingDesc.DepthClipEnable = true;
    noCullingDesc.ScissorEnable = false;
    noCullingDesc.MultisampleEnable = true;
    noCullingDesc.AntialiasedLineEnable = false;

    mpD3dDevice->CreateRasterizerState(&noCullingDesc, &pRaster);
    mpD3dDevContext->RSSetState(pRaster);

    SAFE_RELEASE(pRaster);
}
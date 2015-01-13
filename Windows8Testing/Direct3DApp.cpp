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
    mpD3dImmediateContext(0),
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
    // Relies on device being created to check MSAA, should not createDeviceAndSwapChain below, instead create device, then set up SwapChain
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
        driverTypes[0],                                     // D3D Driver type: Use hardware
        NULL,                                               // HMODULE: handle to DLL software rasterizer
        creationFlags,                                      // D3D11_CREATE_DEVICE_FLAG
        featureLevels,                                      // D3D_FEATURE_LEVEL flags
        ARRAYSIZE(featureLevels),                           // UINT Feature levels
        D3D11_SDK_VERSION,                                  // SDK Version
        &mScd,                                              // IDGXI_SWAP_CHAIN_DESC address
        &mpSwapChain,                                       // Swap chain address
        &mpD3dDevice,                                       // pointer to the device
        &mFeatureLevel,                                     // pointer to feature level support: D3D_FEATURE_LEVEL
        &mpD3dImmediateContext                                    // pointer to the device context
        );

    if (FAILED(hr))
    {
        printf_s("Failed to create device. Error: ");
        MessageBox(mHWnd, L"Failed to Create Device and SwapChain", L"Device Creation Failed", MB_ICONERROR);
        throw(hr);
    }

    //SetSwapChainDesc();
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
        MessageBox(mHWnd, L"Failed to Init Render Target", L"Render Target Failed", MB_ICONERROR);
        throw(hr);
    }

    SetViewPort(SCREEN_WIDTH, SCREEN_HEIGHT);

    SetRasterState();

    InitWorldMatrix();

    hr = InitConstantBuffer();
    if (FAILED(hr))
    {
        MessageBox(mHWnd, L"Failed to Init Constant Buffer", L"CB Init Failed", MB_ICONERROR);
        throw(hr);
    }

    mCube.Init();

    hr = InitVertBuffer();
    if (FAILED(hr))
    {
        MessageBox(mHWnd, L"Failed to Init Vertex Buffer", L"Vertex Buffer Init Failed", MB_ICONERROR);
        throw(hr);
    }

    hr = InitIndexBuffer();
    if (FAILED(hr))
    {
        MessageBox(mHWnd, L"Failed to Init Index Buffer", L"Index Buffer Init Failed", MB_ICONERROR);
        throw(hr);
    }

    hr = CompileShaders();
    if (FAILED(hr))
    {
        MessageBox(mHWnd, L"Failed to Compiile Shader Files", L"Comile Shaders Failed", MB_ICONERROR);
        throw(hr);
    }

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
    mpD3dImmediateContext->OMSetRenderTargets(1, &mpRenderTargetView, NULL);

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

    mpD3dImmediateContext->RSSetViewports(1, &viewport);
}

HRESULT D3DApp::SetSwapChainDesc()
{
    // TODO: Either modify this or create separate function to change the Swap Chain Desc's settings such as MSAA Sample
    
    // Zero memory the structure
    ZeroMemory(&mScd, sizeof(DXGI_SWAP_CHAIN_DESC));

    HRESULT hr;
    
    UINT msaaQuality = 1;
    UINT sample;
    if (mpD3dDevice)
    {
        for (UINT sampleCount = 1; sampleCount < D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; sampleCount++)
        {
            hr = mpD3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, sampleCount, &msaaQuality);

            if (hr != S_OK)
            {
                printf("Do not use this one...");
            }

            if (hr == S_OK)
            {
                std::cout << "Sample count: " << sampleCount << " MaxQualty: " << msaaQuality << std::endl;
            }
        }
    }

    // Set the swap chain structure
    mScd.BufferCount = 1;                                   // Size of back buffer
    mScd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // Set the display buffer format (32 bit format)
    mScd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // Use the surface or resource as an output render only
    mScd.OutputWindow = mHWnd;                              // Handler to the window that will be used
    mScd.SampleDesc.Count = 8;                              // How many multi samples to use
    mScd.SampleDesc.Quality = 0;
    mScd.Windowed = true;                                   // Window mode
    mScd.BufferDesc.Width = SCREEN_WIDTH;                   // Back buffer width
    mScd.BufferDesc.Height = SCREEN_HEIGHT;                 // Backk buffer height
    mScd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // Allow full-screen switching
    mScd.BufferDesc.RefreshRate.Numerator = 60;             // Refresh Rate top
    mScd.BufferDesc.RefreshRate.Denominator = 1;            // Refresh Rate Bottom 60 / 1 = 60FPS

    return S_OK;
}

void D3DApp::CalcFps()
{

}

void D3DApp::Render(float dt)
{
    // TODO: Consider making this more independent. 
    /*  The current program relies too heavily on too many components. 
    What I propose to do is consider making a base class for GameObjects. Each game object will
    be considered items that are viewable in our world, polygonal items in our 3D game. It is a 3D
    shape akin to how Unity's own GameObjects are simple primitives. 
    Along with this method, we want every game object to make a call to the draw function. I propose
    that the GameObject class holds a pointer to the Renderer class and all game objects will be requied
    to have a Render/Draw().
    */
    if (!mpD3dDevice)
    {
        return;
    }
    // Update our time
   // static float t = 0.0f;
   //
   //static ULONGLONG timeStart = 0;
   //ULONGLONG timeCur = GetTickCount64();
   //if (timeStart == 0)
   //    timeStart = timeCur;
   //t = (timeCur - timeStart) / 1000.0f;
   //
    float trans = dt;
    mCube.SetPosition(0, 0, 0);

    // Clear the back buffer to a color
    mpD3dImmediateContext->ClearRenderTargetView(mpRenderTargetView, color);

    m_ScaleMat = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
    m_TranslationMat = DirectX::XMMatrixTranslation(mCube.GetPositionX(), mCube.GetPositionY(), mCube.GetPositionZ());
    m_RotationMat = DirectX::XMMatrixRotationRollPitchYaw(trans, trans * 0.5, trans);

    m_World = DirectX::XMMatrixMultiply(m_ScaleMat, m_RotationMat);
    m_World = DirectX::XMMatrixMultiply(m_World, m_TranslationMat);

    // Transpose our Matrices
    ConstantBuffer cb;

    cb.world = DirectX::XMMatrixTranspose(m_World);
    cb.view = DirectX::XMMatrixTranspose(m_View);
    cb.proj = DirectX::XMMatrixTranspose(m_Projection);
    mpD3dImmediateContext->UpdateSubresource(mConstantBuffer, 0, nullptr, &cb, 0, 0);

    // Clear depth stencil view
    mpD3dImmediateContext->ClearDepthStencilView(mpDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    
    // Set Vertex Shader
    mpD3dImmediateContext->VSSetShader(mpVertexShader, nullptr, 0);
    // Set Constant buffer
    mpD3dImmediateContext->VSSetConstantBuffers(0, 1, &mConstantBuffer);
    // Set Pixel Shader
    mpD3dImmediateContext->PSSetShader(mpPixelShader, nullptr, 0);


    mpD3dImmediateContext->DrawIndexed(36, 0, 0);
    //char str[124] = "Chris Carlos: D3D11 Demo: ";
    char strB[124];
    sprintf_s(strB, "Chris Carlos: %.9f", dt);
    //strcat_s(str, 148, strB);
    //std::puts(str);
    SetWindowTextA(mHWnd, (LPSTR)strB);
    
    // Present the scene
    mpSwapChain->Present(0,0);

    return;

}

void D3DApp::Shutdown(void)
{
    mpSwapChain->SetFullscreenState(false, NULL);
    mpD3dImmediateContext->ClearState();
    SAFE_RELEASE(mpD3dImmediateContext);
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

/***********
* TODO: 
* This function will need to be a bit more
* indeendent. Let us try and make either a separate class,
* or add functionality to this class to set up our buffers
* regardless of how many objects we have to display. I want
* to display more cubes!
* 
* My current approach will be needing to create a base class
* then we shall store these objects into a container of the base objects,
* upon which we will call the independent object's functions that 
* will be in our buffer, and have our function set the memory aside for the object
* based upon the values returns. 
************/

HRESULT D3DApp::InitVertBuffer()
{
    mCube.Init();

    HRESULT hr;

    // Set up buffer description
    D3D11_BUFFER_DESC vbd;
    ZeroMemory(&vbd, sizeof(vbd));
    vbd.Usage = D3D11_USAGE_DEFAULT;
    //vbd.ByteWidth = sizeof(VertexPC) * (sizeof(vertices)/sizeof(vertices[0]));
    vbd.ByteWidth = mCube.GetSizeOfVertexStruct() * mCube.GetSizeOfVertices();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    // Set up subsource data: This describes the data we want to initialize the buffer contents with
    D3D11_SUBRESOURCE_DATA vInitData;
    ZeroMemory(&vInitData, sizeof(vInitData));
    //vInitData.pSysMem = vertices;
    vInitData.pSysMem = mCube.GetPointerToVerticesArray();

    hr = mpD3dDevice->CreateBuffer(
        &vbd,
        &vInitData,
        &mVertexBuffer);

    if (FAILED(hr))
    {
        MessageBox(mHWnd, L"Failed to create Cube Buffer!", L"Buffer Creat Failed", MB_ICONERROR);
        return hr;
    }

    //UINT stride = sizeof(VertexPC);
    UINT offset = 0;
    unsigned int stride = mCube.GetSizeOfVertexStruct();
    // Set Vertex Buffers
    mpD3dImmediateContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

    return S_OK;
}

HRESULT D3DApp::InitIndexBuffer()
{
    HRESULT hr;

    D3D11_BUFFER_DESC ibd;
    ZeroMemory(&ibd, sizeof(ibd));
    ibd.Usage = D3D11_USAGE_DEFAULT;
    //ibd.ByteWidth = sizeof(unsigned int) * (sizeof(indices)/sizeof(indices[0]));
    ibd.ByteWidth = sizeof(unsigned int) * mCube.GetSizeOfIndices();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iInitData;
    //iInitData.pSysMem = indices;
    iInitData.pSysMem = mCube.GetPointerToIndicesArray();
    hr = mpD3dDevice->CreateBuffer(&ibd, &iInitData, &mIndexBuffer);

    if (FAILED(hr))
    {
        MessageBox(mHWnd, L"Failed to create Cube Buffer!", L"INDEX BUFFER Create Failed", MB_ICONERROR);
        return hr;
    }

    // Set Index Buffer 
    mpD3dImmediateContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set Primitive Topology
    mpD3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return S_OK;
}

void D3DApp::InitWorldMatrix()
{
    // World Matrix Init
    m_World = DirectX::XMMatrixIdentity();
    m_View = DirectX::XMMatrixIdentity();
    m_Projection = DirectX::XMMatrixIdentity();
    m_ScaleMat = DirectX::XMMatrixIdentity();
    m_TranslationMat = DirectX::XMMatrixIdentity();

    // Rect struct
    RECT rc;
    GetClientRect(mHWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
    DirectX::XMVECTOR lookAt = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    // Set up view Matrix
    m_View = DirectX::XMMatrixLookAtLH(eyePos, lookAt, up);
    m_Projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, width / (float)height, 0.01f, 1000.0f);
}

HRESULT D3DApp::InitConstantBuffer()
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
        return hr;
    }

    return S_OK;
}

void D3DApp::SetRasterState()
{
    if (!mpD3dImmediateContext)
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
    mpD3dImmediateContext->RSSetState(pRaster);

    SAFE_RELEASE(pRaster);
}

HRESULT D3DApp::CompileFile(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr;

    DWORD dwShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
    dwShaderFlag |= D3DCOMPILE_DEBUG;
    dwShaderFlag |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif 
    ID3DBlob* pErrorBlob = nullptr;

    hr = D3DCompileFromFile(fileName, nullptr, nullptr, entryPoint, shaderModel, dwShaderFlag, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
            pErrorBlob->Release();
            return hr;
        }
    }

    if (pErrorBlob)
    {
        pErrorBlob->Release();
    }

    return S_OK;
}

HRESULT D3DApp::CompileShaders()
{
    LPCSTR profile = "vs_5_0";
    HRESULT hr;

    hr = CompileFile(L"VertexShader.hlsl", "VS", profile, &mpVSBlob);

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

    profile = "ps_5_0";
    hr = CompileFile(L"PixelShader.hlsl", "PS", profile, &mpPSBlob);

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

    //Set Vertex Input Layout
    D3D11_INPUT_ELEMENT_DESC vertDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    UINT numElements = ARRAYSIZE(vertDesc);
    // Set Inputlayout
    hr = mpD3dDevice->CreateInputLayout(vertDesc, numElements/*size of vertDesc*/, mpVSBlob->GetBufferPointer(), mpVSBlob->GetBufferSize(), &mInputLayout);

    if (FAILED(hr))
    {
        MessageBox(mHWnd, L"Failed to create Input Layout", L"Input Layout Create Failed", MB_ICONERROR);
        return hr;
    }

    // Set Input Layout
    mpD3dImmediateContext->IASetInputLayout(mInputLayout);

    return S_OK;
}
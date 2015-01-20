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
    SAFE_RELEASE(m_pSwapChain);
    SAFE_RELEASE(m_pDeviceContext); // Maybe this should be released by the base class //
    SAFE_RELEASE(m_pD3dDevice); // Maybe this should be released by the base class //
    SAFE_RELEASE(m_pRenderTargetView);
    SAFE_RELEASE(m_pDepthStencilView);
    SAFE_RELEASE(m_pConstantBuffer);
    SAFE_RELEASE(m_pVertexBuffer);
    SAFE_RELEASE(m_pIndexBuffer);
    //m_pAppDevice->Shutdown();
}

bool D3DRender::Init(D3DApp *pAppDevice)
{
    color[0] = 0.0f;
    color[1] = 0.2f;
    color[2] = 0.4f;
    color[3] = 1.0f;
    
    HRESULT hr;
    
    if (pAppDevice)
    {
        m_pDeviceContext = pAppDevice->GetDeviceContext();
        if (!m_pDeviceContext)
        {
            return false;
        }
        m_pD3dDevice = pAppDevice->GetDevice();
        if (!m_pD3dDevice)
        {
            return false;
        }
        m_pSwapChain = pAppDevice->GetSwapChain();
        if (!m_pSwapChain)
        {
            return false;
        }
        m_pHwnd = pAppDevice->GetHWND();
        if (!m_pHwnd)
        {
            return false;
        }
    }

    hr = InitRenderTarget();
    if (FAILED(hr))
    {
        MessageBox(*m_pHwnd, L"Failed to Init Render Target", L"Render Target Failed", MB_ICONERROR);
        throw(hr);
    }

    SetViewPort(SCREEN_WIDTH, SCREEN_HEIGHT);

    SetRasterState();

    InitWorldMatrix();

    hr = InitConstantBuffer();
    if (FAILED(hr))
    {
        MessageBox(*m_pHwnd, L"Failed to Init Constant Buffer", L"CB Init Failed", MB_ICONERROR);
        throw(hr);
    }

    hr = CompileShaders();
    if (FAILED(hr))
    {
        MessageBox(*m_pHwnd, L"Failed to Compiile Shader Files", L"Comile Shaders Failed", MB_ICONERROR);
        throw(hr);
    }
    return true;
}

void D3DRender::Render(float dt, std::vector<Object::GameObject*> *pObjects)
{
    if (!m_pD3dDevice)
    {
        return;
    }
    static float trans = 0.0f;
    trans += dt;

    static double totalTime = 0.0f;
    totalTime += dt;
    // Clear the back buffer to a color
    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);

    // CAMERA CLASS FUTURE REFERENCE HERE // 
    /* TODO: We'll create a camera class, that will store these matrices
    (world, proj, view), and every object in our game will pass over it's 
    values that are needed, or the camera will obtain them from all objects.
    We come into our Render() while pass it our camera class, who has 
    a list of objects we will be rendering in view*/
    for (auto i = 0; i < pObjects->size(); i++)
    {
        m_ScaleMat = DirectX::XMMatrixScaling(
            pObjects->at(i)->GetScaleX(), 
            pObjects->at(i)->GetScaleY(),
            pObjects->at(i)->GetScaleZ());
        m_TranslationMat = DirectX::XMMatrixTranslation(
            pObjects->at(i)->GetPosX(),
            pObjects->at(i)->GetPosY(), 
            pObjects->at(i)->GetPosZ());
        m_RotationMat = DirectX::XMMatrixRotationRollPitchYaw(
            pObjects->at(i)->GetRotX() * trans,
            pObjects->at(i)->GetRotY() * trans,
            pObjects->at(i)->GetRotZ() * trans);

        m_World = DirectX::XMMatrixMultiply(m_ScaleMat, m_RotationMat);
        m_World = DirectX::XMMatrixMultiply(m_World, m_TranslationMat);


        //m_ScaleMat = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
        //m_TranslationMat = DirectX::XMMatrixTranslation(mDynCube.GetPositionX(), mDynCube.GetPositionY(), mDynCube.GetPositionZ());
        //m_RotationMat = DirectX::XMMatrixRotationRollPitchYaw(trans, trans * 0.5, trans);

        //m_World = DirectX::XMMatrixMultiply(m_ScaleMat, m_RotationMat);
        //m_World = DirectX::XMMatrixMultiply(m_World, m_TranslationMat);
        // END CAMERA CLASS REF // 
        // Transpose our Matrices
        ConstantBuffer cb;

        cb.world = DirectX::XMMatrixTranspose(m_World);
        cb.view = DirectX::XMMatrixTranspose(m_View);
        cb.proj = DirectX::XMMatrixTranspose(m_Projection);
        m_pDeviceContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

        // Clear depth stencil view
        m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        // Set Vertex Shader
        m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
        // Set Constant buffer
        m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
        // Set Pixel Shader
        m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);

        m_pDeviceContext->DrawIndexed(pObjects->at(i)->GetSizeOfIndexArray(), 0, 0);
    }

    char strB[256];
    sprintf_s(strB, "Chris Carlos: %.4f %.4f FPS %.4f Secs", dt, mFps, totalTime);

    SetWindowTextA(*m_pHwnd, (LPSTR)strB);

    // Present the scene
    m_pSwapChain->Present(0, 0);

    return;
}

void D3DRender::Update(float dt)
{
    //Render(dt);
    CalcFrames(dt);
}

HRESULT D3DRender::InitConstantBuffer()
{
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;

    HRESULT hr;

    hr = m_pD3dDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);

    if (FAILED(hr))
    {
        MessageBox(*m_pHwnd, L"Failed to create Constant Buffer!", L"Constant Buffer Init Failed!", MB_OK);
        return hr;
    }

    return S_OK;
}

HRESULT D3DRender::InitRenderTarget()
{
    if (!m_pD3dDevice)
        return(0);

    HRESULT hr;
    // TODO: Check if this works, else declare as member variable in class //
    ID3D11Texture2D*        pBackBuffer;
    // Get buffer address for the swap chain
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
    {
        printf_s("Failed to obtain back buffer address from swap chain");
        return(hr);
    }

    // Use buffer address now to create the render target to the device
    hr = m_pD3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
    pBackBuffer->Release();

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
    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

    return(hr);
}

HRESULT D3DRender::InitDepthStencilDesc()
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

    // TODO: verify this works else create as a member of the class
    ID3D11Texture2D* pDepthStencilBuffer;
    ZeroMemory(&pDepthStencilBuffer, sizeof(ID3D11Texture2D));

    if (FAILED(hr = m_pD3dDevice->CreateTexture2D(&dsDesc, 0, &pDepthStencilBuffer)))
    {
        MessageBox(*m_pHwnd, L"Failed to create Depth Stencil Buffer", L"ERROR: Depth Stencil Buffer", MB_ICONERROR);
        return(hr);
    }    
    //ID3D11DepthStencilView* pDepthStencilView;

    if (FAILED(hr = m_pD3dDevice->CreateDepthStencilView(pDepthStencilBuffer, 0, &m_pDepthStencilView)))
    {
        MessageBox(*m_pHwnd, L"Failed to create Depth Stencil View", L"ERROR: Depth Stencil View", MB_ICONERROR);
        return(hr);
    }
    SAFE_RELEASE(pDepthStencilBuffer);
    return(hr);
}

void D3DRender::SetViewPort(int width, int height)
{
    if (!m_pD3dDevice)
        return;

    D3D11_VIEWPORT  viewport;

    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    // Set up parameters
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MaxDepth = 1.0f;

    m_pDeviceContext->RSSetViewports(1, &viewport);
}

//HRESULT D3DRender::SetSwapChainDesc()
//{
//    // TODO: Either modify this or create separate function to change the Swap Chain Desc's settings such as MSAA Sample
//    DXGI_SWAP_CHAIN_DESC mScd;
//    // Zero memory the structure
//    ZeroMemory(&mScd, sizeof(DXGI_SWAP_CHAIN_DESC));
//
//    HRESULT hr;
//
//    UINT msaaQuality = 1;
//    UINT sample;
//    if (m_pD3dDevice)
//    {
//        for (UINT sampleCount = 1; sampleCount < D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; sampleCount++)
//        {
//            hr = m_pD3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, sampleCount, &msaaQuality);
//
//            if (hr != S_OK)
//            {
//                printf("Do not use this one...");
//            }
//
//            if (hr == S_OK)
//            {
//                std::cout << "Sample count: " << sampleCount << " MaxQualty: " << msaaQuality << std::endl;
//            }
//        }
//    }
//
//    // Set the swap chain structure
//    mScd.BufferCount = 1;                                   // Size of back buffer
//    mScd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // Set the display buffer format (32 bit format)
//    mScd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // Use the surface or resource as an output render only
//    mScd.OutputWindow = *m_pHwnd;                              // Handler to the window that will be used
//    mScd.SampleDesc.Count = 8;                              // How many multi samples to use
//    mScd.SampleDesc.Quality = 0;
//    mScd.Windowed = true;                                   // Window mode
//    mScd.BufferDesc.Width = SCREEN_WIDTH;                   // Back buffer width
//    mScd.BufferDesc.Height = SCREEN_HEIGHT;                 // Backk buffer height
//    mScd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // Allow full-screen switching
//    mScd.BufferDesc.RefreshRate.Numerator = 60;             // Refresh Rate top
//    mScd.BufferDesc.RefreshRate.Denominator = 1;            // Refresh Rate Bottom 60 / 1 = 60FPS
//
//    return S_OK;
//}

void D3DRender::SetRasterState()
{
    if (!m_pDeviceContext)
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

    m_pD3dDevice->CreateRasterizerState(&noCullingDesc, &pRaster);
    m_pDeviceContext->RSSetState(pRaster);

    SAFE_RELEASE(pRaster);
}

HRESULT D3DRender::AddToVertBuffer(UINT byteWidth, void** dataLocation, UINT str, UINT off)
{
    HRESULT hr;

    // Set up buffer description
    D3D11_BUFFER_DESC vbd;
    ZeroMemory(&vbd, sizeof(vbd));
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = byteWidth;/*DataSize*/
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    // Set up subsource data: This describes the data we want to initialize the buffer contents with
    D3D11_SUBRESOURCE_DATA vInitData;
    ZeroMemory(&vInitData, sizeof(vInitData));
    vInitData.pSysMem = dataLocation;
    //vInitData.pSysMem = mPolyCube.GetPointerToVerticesArray();

    hr = m_pD3dDevice->CreateBuffer(
        &vbd,
        &vInitData,
        &m_pVertexBuffer);

    if (FAILED(hr))
    {
        MessageBox(*m_pHwnd, L"Failed to add object to buffer!", L"Failed to Create Vertex Buffer", MB_ICONERROR);
        return hr;
    }

    //UINT stride = sizeof(VertexPC);
    UINT offset = off;
    unsigned int stride = str;
    // Set Vertex Buffers
    m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    return S_OK;
}

HRESULT D3DRender::InitVertBuffer(/*UINT ByteWidth, void** dataLocation, UINT stride, UINT offset */)
{
    HRESULT hr;

    // Set up buffer description
    D3D11_BUFFER_DESC vbd;
    ZeroMemory(&vbd, sizeof(vbd));
    vbd.Usage = D3D11_USAGE_DEFAULT;
    //vbd.ByteWidth = sizeof(VertexPC) * (sizeof(vertices)/sizeof(vertices[0]));
    vbd.ByteWidth = sizeof(mPolyCube);/*DataSize*///mPolyCube.GetSizeOfVertexStruct() * mPolyCube.GetSizeOfVertices();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    // Set up subsource data: This describes the data we want to initialize the buffer contents with
    D3D11_SUBRESOURCE_DATA vInitData;
    ZeroMemory(&vInitData, sizeof(vInitData));
    //vInitData.pSysMem = vertices;
    vInitData.pSysMem = mPolyCube.GetPointerToVerticesArray();

    hr = m_pD3dDevice->CreateBuffer(
        &vbd,
        &vInitData,
        &m_pVertexBuffer);

    if (FAILED(hr))
    {
        MessageBox(*m_pHwnd, L"Failed to create Cube Buffer!", L"Buffer Create Failed", MB_ICONERROR);
        return hr;
    }

    //UINT stride = sizeof(VertexPC);
    UINT offset = 0;
    unsigned int stride = mPolyCube.GetSizeOfVertexStruct();
    // Set Vertex Buffers
    m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    return S_OK;
}

HRESULT D3DRender::InitIndexBuffer(/*UINT size, void** dataLocation, */)
{
    HRESULT hr;
    // Create Index buffer
    // TODO check if this is bad or good: Create buffer, perform operations, release buffer object //
    ID3D11Buffer* pIndexBuffer;
    D3D11_BUFFER_DESC ibd;
    ZeroMemory(&ibd, sizeof(ibd));
    ibd.Usage = D3D11_USAGE_DEFAULT;
    //ibd.ByteWidth = sizeof(unsigned int) * (sizeof(indices)/sizeof(indices[0]));
    ibd.ByteWidth = sizeof(unsigned int) * mPolyCube.GetSizeOfIndices();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iInitData;
    //iInitData.pSysMem = indices;
    iInitData.pSysMem = mPolyCube.GetPointerToIndicesArray();
    hr = m_pD3dDevice->CreateBuffer(&ibd, &iInitData, &pIndexBuffer);

    if (FAILED(hr))
    {
        MessageBox(*m_pHwnd, L"Failed to create Cube Buffer!", L"INDEX BUFFER Create Failed", MB_ICONERROR);
        return hr;
    }

    // Set Index Buffer 
    m_pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set Primitive Topology
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    SAFE_RELEASE(pIndexBuffer);
    return S_OK;
}

HRESULT D3DRender::AddToIndexBuffer(UINT size, void** dataLocation)
{
    HRESULT hr;
    // Create Index buffer
    // TODO check if this is bad or good: Create buffer, perform operations, release buffer object //
    ID3D11Buffer* pIndexBuffer;
    D3D11_BUFFER_DESC ibd;
    ZeroMemory(&ibd, sizeof(ibd));
    ibd.Usage = D3D11_USAGE_DEFAULT;
    //ibd.ByteWidth = sizeof(unsigned int) * (sizeof(indices)/sizeof(indices[0]));
    ibd.ByteWidth = size;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iInitData;
    //iInitData.pSysMem = indices;
    iInitData.pSysMem = dataLocation;
    hr = m_pD3dDevice->CreateBuffer(&ibd, &iInitData, &pIndexBuffer);

    if (FAILED(hr))
    {
        MessageBox(*m_pHwnd, L"Failed to create Index buffer!", L"Failed to create index buffer!", MB_ICONERROR);
        return hr;
    }

    // Set Index Buffer 
    m_pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set Primitive Topology
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    SAFE_RELEASE(pIndexBuffer);
    return S_OK;
}

HRESULT D3DRender::CompileFile(WCHAR* fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr;

    DWORD dwShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
    dwShaderFlag |= D3DCOMPILE_DEBUG;
    dwShaderFlag |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif 
    //ID3DBlob* pErrorBlob = nullptr;

    hr = D3DCompileFromFile(fileName, nullptr, nullptr, 
        entryPoint, shaderModel, dwShaderFlag, 0, ppBlobOut, &m_pErrorMsgBlob);

    if (FAILED(hr))
    {
        if (m_pErrorMsgBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char*>(m_pErrorMsgBlob->GetBufferPointer()));
            m_pErrorMsgBlob->Release();
            return hr;
        }
    }

    if (m_pErrorMsgBlob)
    {
        m_pErrorMsgBlob->Release();
    }

    return S_OK;
}

HRESULT D3DRender::CompileShaders()
{
    LPCSTR profile = "vs_5_0";
    HRESULT hr;
    // Compile the shader files we will use at run time
    ID3DBlob* pVSBlob;
    ID3DBlob* pPSBlob;
    hr = CompileFile(L"VertexShader.hlsl", "VS", profile, &pVSBlob);

    if (FAILED(hr))
    {
        if (pVSBlob)
        {
            pVSBlob->Release();
        }
    }

    // Create the Vertex Shader
    hr = m_pD3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), 
        pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader);

    if (FAILED(hr))
    {
        MessageBox(*m_pHwnd, L"Failed to create vertex shader", L"Vertex Shader Creation Failed!", MB_ICONERROR);
        pVSBlob->Release();
    }

    profile = "ps_5_0";
    hr = CompileFile(L"PixelShader.hlsl", "PS", profile, &pPSBlob);

    if (FAILED(hr))
    {
        if (pPSBlob)
        {
            pPSBlob->Release();
        }
    }

    hr = m_pD3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
        pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader);

    if (FAILED(hr))
    {
        MessageBox(*m_pHwnd, L"Failed to create pixel shader", L"Pixel shader creation failed!", MB_ICONERROR);
        pPSBlob->Release();
    }

    //Set Vertex Input Layout
    D3D11_INPUT_ELEMENT_DESC vertDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    // TODO: Create inputLayout
    ID3D11InputLayout* pInputLayout;
    UINT numElements = ARRAYSIZE(vertDesc);
    // Set Inputlayout
    hr = m_pD3dDevice->CreateInputLayout(vertDesc, numElements/*size of vertDesc*/, 
        pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &pInputLayout);

    if (FAILED(hr))
    {
        MessageBox(*m_pHwnd, L"Failed to create Input Layout", L"Input Layout Create Failed", MB_ICONERROR);
        return hr;
    }

    // Set Input Layout
    m_pDeviceContext->IASetInputLayout(pInputLayout);

    SAFE_RELEASE(pVSBlob);
    SAFE_RELEASE(pPSBlob);
    SAFE_RELEASE(m_pErrorMsgBlob);
    return S_OK;
}

void D3DRender::InitWorldMatrix()
{
    // World Matrix Init
    m_World = DirectX::XMMatrixIdentity();
    m_View = DirectX::XMMatrixIdentity();
    m_Projection = DirectX::XMMatrixIdentity();
    m_ScaleMat = DirectX::XMMatrixIdentity();
    m_TranslationMat = DirectX::XMMatrixIdentity();

    // Rect struct
    RECT rc;
    GetClientRect(*m_pHwnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
    DirectX::XMVECTOR lookAt = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    // Set up view Matrix
    m_View = DirectX::XMMatrixLookAtLH(eyePos, lookAt, up);
    m_Projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, width / (float)height, 0.01f, 1000.0f);
}

void D3DRender::CalcFrames(float dt)
{
    // take in current time and count it up
    static float totalTime = 0.0f;
    static int numFrames = 0;
    totalTime += dt;
    if (totalTime >= 1.0f)
    {
        mFps = numFrames;
        numFrames = 0;
        totalTime = 0.0f;
    }
    else
    {
        numFrames++;
    }
}




#include "pch.h"


#include "HvyInst.h"
#include "..\Common\DirectXHelper.h"


using namespace HvyDXBase; 
using namespace DirectX;
using namespace Windows::Foundation;







HvyInst::HvyInst(
    std::shared_ptr<DX::DeviceResources>    const &     deviceResources, 
    Microsoft::WRL::ComPtr<ID3D11Buffer>    const &     MVP_Buffer
) :

    m_deviceResources(deviceResources), 
    m_loadingComplete(false)
{

    //  compiles okay :   ID3D11Buffer* const *   gv_fail = MVP_Buffer.GetAddressOf();



    gv_fail = MVP_Buffer.GetAddressOf();

    
    CreateDeviceDepResources();  
}





















void HvyInst::LoxodromeBeta(std::vector<VHG_Instance>    *p_vect_Instances, double beta)
{
    double const lam0 = 0.00;



    double const epsilon = 0.02;
    double const phiInit = -3.14159 / 2.0 + epsilon;
    double const phiFinal = +3.14159 / 2.0 - epsilon;

    int const nDivs = 1024;  //  use nDivs = 1024;

    double anglePhi = 0.00;  // polar angle phi:  [-pi/2,  +pi/2];

    double lambda = 0.00;  //  azimuthal angle lambda:  [0, 2pi);


    XMFLOAT3 zeroF3 = { 0.0f, 0.0f, 0.0f };
    VHG_Instance current_instance = { zeroF3, zeroF3 };


    double radMult = 1.8;  // using  2.00; puts nodes outside of sphere;


    double psi = 0.00;

    //  
    //  Note that an artificial multiplier "omega" (4.0) has been added to expressions
    //  for current_instance.inst_pos.x and .z. It doesn't belong to the rhumb line per se, 
    //  but it causes the loxodrome to wind more rapidly for a pleasing aesthetic effect. 
    //      

    double omega = 4.0; // artificial angular speed = 4.00;



    for (int idxLoop = 0; idxLoop < nDivs; idxLoop++)
    {
        anglePhi = phiInit + idxLoop * (phiFinal - phiInit) / (float)nDivs;

        double fact1 = std::atanh(sin(anglePhi)) - std::atanh(sin(phiInit)); 

        lambda = tan(beta) * fact1 + lam0;

        psi = (lambda - lam0) / std::tan(beta) + std::atanh(sin(phiInit));

        current_instance.inst_pos.x = (float)(radMult * cos(omega * lambda) / std::cosh(psi));
        current_instance.inst_pos.z = (float)(radMult * sin(omega * lambda) / std::cosh(psi));
        current_instance.inst_pos.y = (float)(radMult * sinh(psi) / std::cosh(psi)); 

        current_instance.inst_attributes.y = 3.f;
            
        p_vect_Instances->push_back(current_instance);
    }
}

















  

void HvyDXBase::HvyInst::LerpFlechette(
    std::vector<VHG_Instance>    *p_vect_Instances,
    DirectX::XMVECTOR const &     p_endPointXMV,
    uint32_t                      idx
)
{
    const uint32_t nFlechetteNodes = 256;  // formerly nFlechetteNodes = 48;

    XMFLOAT3 cZero = { 0.0f, 0.0f, 0.0f };

    XMVECTOR vOrigin = XMLoadFloat3(&cZero);



    VHG_Instance current_instance = { cZero, cZero };



    for (uint32_t iAB = 1; iAB <= nFlechetteNodes; iAB++)
    {
            float fParam = (float)iAB / (float)nFlechetteNodes;

            XMVECTOR xmvLerp = XMVectorLerp(p_endPointXMV, vOrigin, fParam);
            XMFLOAT3 f3Lerp;
            XMStoreFloat3(&f3Lerp, xmvLerp);

            current_instance.inst_pos = f3Lerp;
            current_instance.inst_attributes.x = 2 + idx * 100.f;
            current_instance.inst_attributes.y = 3.f;

            p_vect_Instances->push_back(current_instance);
    }
}















uint32_t HvyInst::TriflecheMesh(std::vector<VHG_Instance>    *p_vect_Instances)
{
    float flr = 1.f;

    flr = 2.2f;


    XMFLOAT3 endPointF3 = { flr, 0.f, 0.f };
    XMVECTOR endPointXMV = XMLoadFloat3(&endPointF3);
    LerpFlechette(p_vect_Instances, endPointXMV, 1); 

    endPointF3 = { 0.f, flr, 0.f };
    endPointXMV = XMLoadFloat3(&endPointF3);
    LerpFlechette(p_vect_Instances, endPointXMV, 2); 

    endPointF3 = { 0.f, 0.f, flr };
    endPointXMV = XMLoadFloat3(&endPointF3);
    LerpFlechette(p_vect_Instances, endPointXMV, 3); 

    UINT card = (uint32_t)p_vect_Instances->size();
    return card;
}
















void HvyInst::TriflecheCreateInstanceBuffer()
{
    std::vector<VHG_Instance>           *ptr_vect_Instances = nullptr;

    ptr_vect_Instances = new std::vector<VHG_Instance>;


    double beta = 0.00; 
    // double const incrementBeta = 0.0007;
    double const incrementBeta = 0.0021;

    // for (int nDromes = -6; nDromes < 7; nDromes++)  //  use; 

    for (int nDromes = 0; nDromes < 1; nDromes++)  //  use 19 dromes;
    {
        //  
        //  Choice of beta 
        //     beta = 2.1 really opens up the bands artistically.
        // 
        //      beta = 2.3 works fine; 
        //      beta = 2.5 works great; 
        //      beta = 2.6 optimal: ribbon punctured by the z-axis!!!
        //      beta = 2.7 will flip the entire ribbon pi radians azimuthally!!!!


        beta = 2.605 + nDromes * incrementBeta;  // Use beta center = 2.605!!!
    


        LoxodromeBeta(ptr_vect_Instances, beta);
    }
    
    uint32_t additional_nodes = TriflecheMesh(ptr_vect_Instances); // keep;
    
    TriflecheInstanceCount = (uint32_t)ptr_vect_Instances->size(); // keep; 



    D3D11_SUBRESOURCE_DATA instanceBufferData = { 0 };
    ZeroMemory(&instanceBufferData, sizeof(instanceBufferData));
    instanceBufferData.pSysMem = &((*ptr_vect_Instances)[0]);
    instanceBufferData.SysMemPitch = 0;
    instanceBufferData.SysMemSlicePitch = 0;


    D3D11_BUFFER_DESC instanceBufferDesc;
    ZeroMemory(&instanceBufferDesc, sizeof(instanceBufferDesc));
    instanceBufferDesc.ByteWidth = (UINT)(TriflecheInstanceCount * sizeof(VHG_Instance));
    instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    instanceBufferDesc.MiscFlags = 0;


    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &instanceBufferDesc, &instanceBufferData, &TriflecheInstanceBuffer)
    );

    delete ptr_vect_Instances;
    ptr_vect_Instances = nullptr;
}




















void HvyInst::TriflecheRender(void)
{
    auto d3d_context_3d = m_deviceResources->GetD3DDeviceContext();


    UINT                     instance_strides[2];
    UINT                     instance_offsets[2];
    ID3D11Buffer            *bufferPointers[2];

    // Set the buffer strides 

    instance_strides[0] = sizeof(VHG_Vertex_PosTex);
    instance_strides[1] = sizeof(VHG_Instance);



    // Set the buffer offsets 

    instance_offsets[0] = 0;
    instance_offsets[1] = 0;




    // Set the array of pointers to the vertex and instance buffers

    bufferPointers[0] = ProtoCube_VertexBuffer.Get();
    bufferPointers[1] = TriflecheInstanceBuffer.Get();



    // Set both the vertex buffer and the instance buffer in the same call: 

    d3d_context_3d->IASetVertexBuffers(0, 2, bufferPointers, instance_strides, instance_offsets);


    d3d_context_3d->IASetIndexBuffer(ProtoCube_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);




    d3d_context_3d->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



    d3d_context_3d->IASetInputLayout(ProtoCube_InputLayout.Get());



    d3d_context_3d->RSSetState(e_RasterizerStateSolid.Get());



    d3d_context_3d->VSSetShader(instance_vertexShader.Get(), nullptr, 0);




    //  TODO : d3d_context_3d->VSSetConstantBuffers1(0, 1, m_conbuf_MVP_Buffer.GetAddressOf(), nullptr, nullptr);


    d3d_context_3d->VSSetConstantBuffers1(0, 1, gv_fail, nullptr, nullptr);


    d3d_context_3d->PSSetShader(instance_PixelShader.Get(), nullptr, 0);

    d3d_context_3d->PSSetShaderResources(0, 1, e_srv_axis_x.GetAddressOf()); // slot zero;
    d3d_context_3d->PSSetShaderResources(1, 1, e_srv_axis_y.GetAddressOf()); // slot one;
    d3d_context_3d->PSSetShaderResources(2, 1, e_srv_axis_z.GetAddressOf()); // slot two;

    d3d_context_3d->PSSetSamplers(0, 1, e_SamplerStateGeneric.GetAddressOf());




    d3d_context_3d->DrawIndexedInstanced(
        ProtoCube_IndexCount,
        TriflecheInstanceCount,
        0,  //   start index location is zero; 
        0,  //   base vertex location is zero; 
        0   //   start instance location is zero; 
    );
}



























void HvyInst::DefineTheProtoCube()
{
    // assert((GHV_OPTION_COORD_CHIRALITY) == (GHV_OPTION_CHIRALITY_SINISTRAL));


    std::vector<VHG_Vertex_PosTex> ThePrototypeCube =
    {
        //  
        //     the +y face: 
        // 
        { XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT2(1.f, 0.00f) },
        { XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT2(0.f, 0.00f) },
        { XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT2(0.f, 1.f) },
        { XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT2(1.f, 1.f) },

        //  
        //     the -y face:  
        //  
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.f, 0.00f) },
        { XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.00f) },
        { XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT2(1.0f,  1.f) },
        { XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT2(0.f, 1.f) },

        // 
        //    the  -x face: 
        //   
        { XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT2(0.00f, 1.f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.f,   1.f) },
        { XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT2(1.f,   0.f) },
        { XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT2(0.00f, 0.f) },

        // 
        //    the +x face: 
        //   
        { XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT2(1.f,    1.f) },
        { XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT2(0.00f,  1.f) },
        { XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT2(0.00f,  0.00f) },
        { XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT2(1.f,    0.00f) },

        //  
        //   the front face is the -z face: 
        //  
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.f,     1.f) },   //  16
        { XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT2(1.00f,   1.f) },   //  17 
        { XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT2(1.00f,   0.f) },   //  18 
        { XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT2(0.f,     0.f) },   //  19  

        //  
        //      the back face is the +z face: 
        //  
        { XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT2(1.f, 1.f) },
        { XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT2(0.f, 1.f) },
        { XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT2(0.f, 0.f) },
        { XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT2(1.f, 0.f) }

    };


    //   
    //     Choosing good size for VHG_Scale...
    //   
    //          Using 0.01 is just a bit too much;
    //          Try  0.008f; 



    HvyDXBase::VHG_Scale<VHG_Vertex_PosTex>   gv_scaleObj(0.007f);  // use 0.02f;

    gv_scaleObj.posApply(ThePrototypeCube);




    size_t a_required_allocation = sizeof(VHG_Vertex_PosTex) * (uint32)ThePrototypeCube.size();
        


    D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
    ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
    vertexBufferData.pSysMem = &(ThePrototypeCube[0]);
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;


    CD3D11_BUFFER_DESC vertexBufferDesc(
        (UINT)a_required_allocation,
        D3D11_BIND_VERTEX_BUFFER
    );

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &vertexBufferDesc,
            &vertexBufferData,
            &ProtoCube_VertexBuffer
        ));


    //   
    //     Don't try this without the Index Buffer
    // 

    static const uint32_t cubeIndices[] =
    {
        3,1,0,  		2,1,3,
        6,4,5,  		7,4,6,
        11,9,8,  		10,9,11,
        14,12,13,  		15,12,14,
        19,17,16,  		18,17,19,
        22,20,21,  		23,20,22
    };

    ProtoCube_IndexCount = ARRAYSIZE(cubeIndices);


    D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
    ZeroMemory(&indexBufferData, sizeof(indexBufferData));
    indexBufferData.pSysMem = cubeIndices;
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;

    CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);

    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateBuffer(
            &indexBufferDesc, &indexBufferData, &ProtoCube_IndexBuffer)
    );
}











void HvyInst::CreateSamplerStateGeneric()
{
    D3D11_SAMPLER_DESC sampDescrGeneric;
    ZeroMemory(&sampDescrGeneric, sizeof(sampDescrGeneric));


    sampDescrGeneric.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;


    //  To render the bitmap via pixel shader use D3D11_TEXTURE_ADDRESS_CLAMP:

    sampDescrGeneric.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDescrGeneric.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDescrGeneric.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;


    sampDescrGeneric.ComparisonFunc = D3D11_COMPARISON_NEVER;

    sampDescrGeneric.MinLOD = 0;
    sampDescrGeneric.MaxLOD = D3D11_FLOAT32_MAX;


    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateSamplerState(
            &sampDescrGeneric,
            e_SamplerStateGeneric.ReleaseAndGetAddressOf()
        )
    );
}











void HvyInst::CreateRasterizerWireframe()
{
    D3D11_RASTERIZER_DESC   rasterizer_description;
    ZeroMemory(&rasterizer_description, sizeof(rasterizer_description));

    rasterizer_description.MultisampleEnable = FALSE;


    rasterizer_description.FillMode = D3D11_FILL_WIREFRAME;
    rasterizer_description.CullMode = D3D11_CULL_NONE;


    rasterizer_description.FrontCounterClockwise = FALSE;


    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateRasterizerState(
            &rasterizer_description,
            e_RasterizerStateWireframe.ReleaseAndGetAddressOf()
        )
    );
}










void HvyInst::CreateRasterizerSolid()
{
    D3D11_RASTERIZER_DESC   rasterizer_description;
    ZeroMemory(&rasterizer_description, sizeof(rasterizer_description));

    rasterizer_description.MultisampleEnable = FALSE;


    rasterizer_description.FillMode = D3D11_FILL_SOLID;
    rasterizer_description.CullMode = D3D11_CULL_BACK;


    rasterizer_description.FrontCounterClockwise = FALSE;


    DX::ThrowIfFailed(
        m_deviceResources->GetD3DDevice()->CreateRasterizerState(
            &rasterizer_description,
            e_RasterizerStateSolid.ReleaseAndGetAddressOf()
        )
    );
}












void HvyInst::CreateDeviceDepResources()
{

    CreateRasterizerSolid(); 

    CreateRasterizerWireframe(); 

    CreateSamplerStateGeneric();



    Microsoft::WRL::ComPtr<ID3D11Resource>  temp_resource;


    DX::ThrowIfFailed(CreateWICTextureFromFile(m_deviceResources->GetD3DDevice(),
        L"Assets\\a_axis_x.png",
        temp_resource.ReleaseAndGetAddressOf(),
        e_srv_axis_x.ReleaseAndGetAddressOf(), 0)
    );


    DX::ThrowIfFailed(CreateWICTextureFromFile(m_deviceResources->GetD3DDevice(),
        L"Assets\\a_axis_y.png",
        temp_resource.ReleaseAndGetAddressOf(),
        e_srv_axis_y.ReleaseAndGetAddressOf(), 0)
    );


    DX::ThrowIfFailed(CreateWICTextureFromFile(m_deviceResources->GetD3DDevice(),
        L"Assets\\a_axis_z.png",
        temp_resource.ReleaseAndGetAddressOf(),
        e_srv_axis_z.ReleaseAndGetAddressOf(), 0)
    );




    auto loadVS_Instance_Task = DX::ReadDataAsync(L"ghv_Instancing_VertexShader.cso");
    auto loadPS_Instance_Task = DX::ReadDataAsync(L"ghv_Instancing_PixelShader.cso");





    auto createVS_Instance_Task = loadVS_Instance_Task.then([this](const std::vector<byte>& fileData)
        {
            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateVertexShader(
                    &fileData[0], fileData.size(), nullptr, &instance_vertexShader));



            static const D3D11_INPUT_ELEMENT_DESC vertexDescInstance[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,    D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0,    D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
                { "TEXCOORD", 1, DXGI_FORMAT_R32G32B32_FLOAT, 1,    D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
                { "TEXCOORD", 2, DXGI_FORMAT_R32G32B32_FLOAT, 1,    D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
            };


            DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(
                vertexDescInstance,
                ARRAYSIZE(vertexDescInstance),
                &fileData[0], fileData.size(),
                &ProtoCube_InputLayout
            )
            );

        });







    auto createPS_Instance_Task = loadPS_Instance_Task.then([this](const std::vector<byte>& fileData)
    {
            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreatePixelShader(
                    &fileData[0], fileData.size(), nullptr, &instance_PixelShader));

    });









    auto createCubeTask = (createPS_Instance_Task && createVS_Instance_Task).then([this]()
    {
            
            //  Grid_MeshGridFloor();


            DefineTheProtoCube(); // required by both MeshFlatSpiral and MeshConnector;



            TriflecheCreateInstanceBuffer();



    });







    createCubeTask.then([this]() {
        m_loadingComplete = true;
        });











}














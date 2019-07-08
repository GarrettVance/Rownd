#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"

#include "arcball.h"



using namespace Rownd;

using namespace DirectX;
using namespace Windows::Foundation;





double                   g0_total_seconds = 0.0;
double                   g0_total_sec_stop = 0.0;
double                   g0_total_sec_resume = 0.0;
double                   g0_eff_total_seconds = 0.0;







Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :

	m_loadingComplete(false),



	// m_degreesPerSecond(45),

	m_degreesPerSecond(25),



	Sphere1_indexCount(0),


    e_WholeModelRotationEnabled(true), 




	m_deviceResources(deviceResources)

{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

    e_keyboard = std::make_unique<DirectX::Keyboard>(); 
    e_keyboard->SetWindow(Windows::UI::Core::CoreWindow::GetForCurrentThread());

    e_mouse = std::make_unique<DirectX::Mouse>(); 
    e_mouse->SetWindow(Windows::UI::Core::CoreWindow::GetForCurrentThread());





    e_ptrHvyInstancer = new HvyDXBase::HvyInst(deviceResources, m_constantBuffer); 

    e_RotationAxis = new HvyDXBase::RAxis(deviceResources, m_constantBuffer); 




    ArcBall_Init();

    HVect shoeCenterLogical = { 0.0f, 0.0f, 0.0f, 1.0f };

    double shoeRadiusLogical = 1.f;  //  range 0.25 to 1.25 ?

    ArcBall_Place(shoeCenterLogical, shoeRadiusLogical);

    ArcBall_UseSet(NoAxes); 




}
















void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.




#ifdef GHV_OPTION_DEXTRAL
    XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH( fovAngleY, aspectRatio, 0.01f, 100.0f );
#else

    //  Sinistral then: 
    XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH( fovAngleY, aspectRatio, 0.01f, 100.0f );

#endif





	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.

	// static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };



	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };




#ifdef GHV_OPTION_DEXTRAL

	static const XMVECTORF32 at = { 0.2f, 0.007f, 0.3f, 0.0f };
	static const XMVECTORF32 eye = { 0.0f, 0.7f, 1.5f, 0.0f };
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
#else

    //      Sinistral:   


    //   user is INSIDE the sphere, looking out to the horizon: 
	// static const XMVECTORF32 eye = { 0.0f, 0.3f, 0.5f, 0.0f };  // user is inside the spinning sphere...
	// static const XMVECTORF32 at = { 0.2f, +0.10f, 0.3f, 0.0f };





    //  looking down from above is best: 
	static const XMVECTORF32 eye = { 0.0f, 2.3f, -2.5f, 0.0f };   //  increased the y-component to increase altitude of camera;

    
    // undo ? static const XMVECTORF32 at = { 0.2f, 0.007f, 0.3f, 0.0f };

    static const XMVECTORF32 at = { 0.8f, 0.007f, 0.3f, 0.0f };




    //  looking up from below: 
	// static const XMVECTORF32 eye = { 0.0f, -2.f, -2.5f, 0.0f };   //  temporarily decrease the altitude of camera (by using negative y-component);
	// static const XMVECTORF32 at = { 0.2f, 0.007f, 0.3f, 0.0f };



	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));
#endif

}










D2D1_POINT_2F Sample3DSceneRenderer::g_getCentroidPhys()
{

    Windows::Foundation::Size   renderTargetPixels = m_deviceResources->GetOutputSize();

    float halfWidthRT = renderTargetPixels.Width / 2.f;

    float halfHeightRT = renderTargetPixels.Height / 2.f;

    D2D1_POINT_2F   centroid_2f = D2D1::Point2F(halfWidthRT, halfHeightRT);

    return centroid_2f;
}




float Sample3DSceneRenderer::g_cdisk_phys_radius()
{
    //  Half-height of the RenderTarget is the optimal 
    //  radius for the Poincare Disk (in Physical Coords aka pixels): 

    Windows::Foundation::Size   renderTargetPixels = m_deviceResources->GetOutputSize();
    return renderTargetPixels.Height / 2.0f;
}

















std::complex<double> Sample3DSceneRenderer::LogicalFromPhysical(std::complex<double> p_physical)
{

    double x_logical = (p_physical.real() - g_getCentroidPhys().x) / g_cdisk_phys_radius();

    double y_logical = (g_getCentroidPhys().y - p_physical.imag()) / g_cdisk_phys_radius(); // signum!!!

    return std::complex<double>(x_logical, y_logical);
}








void Sample3DSceneRenderer::gvMouseHandler()
{
    DirectX::Mouse::State    maus_state = e_mouse->GetState();

    e_mouseTracker.Update(maus_state);

    static bool released = true;




    if (e_mouseTracker.leftButton == Mouse::ButtonStateTracker::RELEASED)
    {
        released = true;

        ArcBall_EndDrag();
    }


    if (maus_state.leftButton == true)
    {
        if (released)
        {
            released = false;

            ArcBall_BeginDrag();
        }
    }


    //   Now regardless of whether or not any button was clicked,
    //   the following code must update mouse position constantly!!!


    std::complex<double> mouse_hover_physical = std::complex<double>(1.000 * maus_state.x, 1.000 * maus_state.y);

    std::complex<double> mouse_hover_logical = LogicalFromPhysical(mouse_hover_physical);

    float xNow = (float)mouse_hover_logical.real();

    float yNow = (float)mouse_hover_logical.imag();


    HVect   vNow = { xNow, yNow, 0.f, 0.f };

    ArcBall_Mouse(vNow);  //  CRUCIAL; 

    ArcBall_Update();


}







void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{

    gvMouseHandler(); 


    DirectX::Keyboard::State           kb = e_keyboard->GetState();



    if (kb.O)
    {
        //   A request to resume the rotation: 
        if (e_WholeModelRotationEnabled == false)
        {
            //  Since timer parameter is "const", cannot call timer.ResetElapsedTime(); 


            g0_total_sec_resume = g0_total_seconds;




            e_WholeModelRotationEnabled = true;

        }
    }



    if (kb.K)
    {
        //    Request to stop the rotation: 

        if (e_WholeModelRotationEnabled == true)
        {
            e_WholeModelRotationEnabled = false;

            g0_total_sec_stop = g0_eff_total_seconds;

        }
    }


    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



    g0_total_seconds = timer.GetTotalSeconds();

    float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);

    double total_sec = 0.f;



    if (e_WholeModelRotationEnabled)
    {
        total_sec = g0_total_seconds - (g0_total_sec_resume - g0_total_sec_stop);  // perfect!!!

        g0_eff_total_seconds = total_sec;
    }
    else
    {
        total_sec = g0_total_sec_stop;
    }

    double total_radians = total_sec * radiansPerSecond;
    float proper_radians = static_cast<float>(fmod(total_radians, XM_2PI));


    Rotate(proper_radians);
}








void Sample3DSceneRenderer::Rotate(float radians)
{
    float sca = 0.8f; // scaling factor xyz; 


    HMatrix shoeRotationMatrix;

    ArcBall_Value(shoeRotationMatrix);

    DirectX::SimpleMath::Matrix modelRotationMatrix =
    {
        (float)shoeRotationMatrix[0][0],
        (float)shoeRotationMatrix[0][1],
        (float)shoeRotationMatrix[0][2],
        (float)shoeRotationMatrix[0][3],
        (float)shoeRotationMatrix[1][0],
        (float)shoeRotationMatrix[1][1],
        (float)shoeRotationMatrix[1][2],
        (float)shoeRotationMatrix[1][3],
        (float)shoeRotationMatrix[2][0],
        (float)shoeRotationMatrix[2][1],
        (float)shoeRotationMatrix[2][2],
        (float)shoeRotationMatrix[2][3],
        (float)shoeRotationMatrix[3][0],
        (float)shoeRotationMatrix[3][1],
        (float)shoeRotationMatrix[3][2],
        (float)shoeRotationMatrix[3][3],
    };

    // modelRotationMatrix = XMMatrixTranspose(modelRotationMatrix); 

    //      
    //  Compute the axis of rotation, if there is one:
    //      

    float u1 = shoeRotationMatrix[2][1] - shoeRotationMatrix[1][2];

    float u2 = shoeRotationMatrix[0][2] - shoeRotationMatrix[2][0];

    float u3 = shoeRotationMatrix[1][0] - shoeRotationMatrix[0][1];

    uu_RotationAxis = XMVectorSet(u1, u2, u3, 0.0); 

    if (XMVector3Equal(uu_RotationAxis, XMVectorZero()))
    {
        uu_RotationAxis = XMVectorSet(0.05 + u1, 0.05 + u2, 0.05 + u3, 0.0); 
    }



    XMStoreFloat4x4(
        &m_constantBufferData.model,
        XMMatrixTranspose(
            // modelRotationMatrix * XMMatrixScaling(sca, sca, sca)
            XMMatrixRotationAxis(uu_RotationAxis, radians) * XMMatrixScaling(sca, sca, sca)
        )
    );




  
    this->e_RotationAxis->UpdateConnector(uu_RotationAxis);


}












void Sample3DSceneRenderer::Render()
{
	if (!m_loadingComplete) { return; }





	auto context = m_deviceResources->GetD3DDeviceContext();


	context->UpdateSubresource1( m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0 );




    this->TexCoSphereRender();


    e_ptrHvyInstancer->TriflecheRender();


    e_RotationAxis->TriflecheRender();
}


























void Sample3DSceneRenderer::CreateDeviceDependentResources()
{


    this->TexCoSphereDDSTextureFromFile(); 


    this->TexCoSphereCreateRasterizerState();


    this->TexCoSphereCreateSamplerStateGeneric();


    this->TexCoSphereCreateBlendState();  // allows visibility into interior of sphere...







    //    The ZMesh project relies on Chuck Walbourn's DirectXTK 
   //    for the CreateWICTextureFromFile() method

    Microsoft::WRL::ComPtr<ID3D11Resource>  temp_resource; // for memory leak;

    wchar_t file_path_to_image[] = L"Assets\\a_hyperbolic.png";

    DX::ThrowIfFailed(
        CreateWICTextureFromFile(
            m_deviceResources->GetD3DDevice(),  //  m_d3dDevice.Get(), 
            file_path_to_image,
            temp_resource.ReleaseAndGetAddressOf(),
            Sphere1_TextureSRV.ReleaseAndGetAddressOf(),
            0)
    );





    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%





	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");







	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) 
    {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&Sphere1_vertexShader
				)
			);


		static const D3D11_INPUT_ELEMENT_DESC vertexDesc [] =
		{
			{ "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                               D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",   1, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&Sphere1_inputLayout
				)
			);
	});
















	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) 
    {

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&Sphere1_pixelShader
				)
			);




		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer) , D3D11_BIND_CONSTANT_BUFFER);


		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
				)
			);
	});
    









	auto createCubeTask = (createPSTask && createVSTask).then([this] () 
    {
        //  Mesh_SimplestPossibleCube();

        TexCoSphereCreateVertexBuffer();
    
    });







	
	createCubeTask.then([this] () 
    {

        //  TODO:  need to also consider the "loadingComplete" flag in the HvyInst class!!!


		m_loadingComplete = true;
	});
}
















void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;



	m_constantBuffer.Reset();



	Sphere1_vertexShader.Reset();
	Sphere1_inputLayout.Reset();
	Sphere1_pixelShader.Reset();
	Sphere1_vertexBuffer.Reset();
	Sphere1_indexBuffer.Reset();
}















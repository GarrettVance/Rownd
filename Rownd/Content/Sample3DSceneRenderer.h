#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"



#include "HvyInst.h"

#include "RAxis.h"



namespace Rownd
{

	class VHG_Spherolux
	{
    public: 
        VHG_Spherolux(): 
            position(DirectX::XMFLOAT3(0.f, 0.f, 0.f)), 
            normal(DirectX::XMFLOAT3(0.f, 0.f, 0.f)), 
            texcoGlobal(DirectX::XMFLOAT2(0.f, 0.f)),
            texcoLocal(DirectX::XMFLOAT2(0.f, 0.f)) 
        {}



        VHG_Spherolux(
            DirectX::XMFLOAT3 pPosition,  
            DirectX::XMFLOAT3 pNormal,  
            DirectX::XMFLOAT2 pTexcoGlobal,
            DirectX::XMFLOAT2 pTexcoLocal
        ): 
            position(DirectX::XMFLOAT3(pPosition.x, pPosition.y, pPosition.z)), 
            normal(DirectX::XMFLOAT3(pNormal.x, pNormal.y, pNormal.z)), 
            texcoGlobal(DirectX::XMFLOAT2(pTexcoGlobal.x, pTexcoGlobal.y)),
            texcoLocal(DirectX::XMFLOAT2(pTexcoLocal.x, pTexcoLocal.y)) 
        {}


        //  ghv : TODO: copy ctor...



		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoGlobal;
		DirectX::XMFLOAT2 texcoLocal;
	};















	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);


		void Render();

	public:


        static Rownd::VHG_Spherolux Sample3DSceneRenderer::ComputeTextureCoordinates(
            float pThetaColatitude,
            float pLambdaLongitude,
            DirectX::XMFLOAT3 const & pSCentre,
            float pSRadius, 
            float uLocal, 
            float vLocal
        );




	private:

		void                TexCoSphereRender();
        void                TexCoSphereCreateSamplerStateGeneric();
        void                TexCoSphereCreateRasterizerState();

        void                TexCoSphereCreateBlendState();
        void                TexCoSphereDDSTextureFromFile(void); 

        void                TexCoSphereCreateVertexBuffer(); 





        D2D1_POINT_2F               g_getCentroidPhys();
        float                       g_cdisk_phys_radius();
        std::complex<double>        LogicalFromPhysical(std::complex<double> p_physical);




        void                gvMouseHandler();



		void Rotate(float radians);



	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources>                    m_deviceResources;

		Microsoft::WRL::ComPtr<ID3D11Buffer>		            m_constantBuffer;
		ModelViewProjectionConstantBuffer	                    m_constantBufferData;







        HvyDXBase::HvyInst *                                    e_ptrHvyInstancer;

        HvyDXBase::RAxis*                                       e_RotationAxis;

        DirectX::XMVECTOR                                       uu_RotationAxis;






		Microsoft::WRL::ComPtr<ID3D11InputLayout>	            Sphere1_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		            Sphere1_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		            Sphere1_indexBuffer;

		Microsoft::WRL::ComPtr<ID3D11VertexShader>	            Sphere1_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	            Sphere1_pixelShader;


        Microsoft::WRL::ComPtr<ID3D11RasterizerState>           Sphere1_RasterizerState;  //  e_rasterizer_state;
        Microsoft::WRL::ComPtr<ID3D11SamplerState>				Sphere1_TextureSamplerState; 
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		Sphere1_TextureSRV;


        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>        Sphere1_DDS_A_SRV;      //  e_heightmap_srv;
        Microsoft::WRL::ComPtr<ID3D11Texture2D>                 Sphere1_DDS_A_Texture;  //  e_heightmap_texture;


        Microsoft::WRL::ComPtr<ID3D11BlendState>                Sphere1_BlendState;
		uint32	                                                Sphere1_indexCount;


        std::unique_ptr<DirectX::Keyboard>                      e_keyboard;
        std::unique_ptr<DirectX::Mouse>                         e_mouse;
        DirectX::Mouse::ButtonStateTracker                      e_mouseTracker;
        int                                                     e_mouseWheel;
        bool                                                    e_WholeModelRotationEnabled; 
        float                                                   e_WholeModelRotationStoppedAngle; 
        float                                                   e_RotationInstantaneousRadians;





		bool	m_loadingComplete;
		float	m_degreesPerSecond;

	};
}


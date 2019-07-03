#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"






namespace HvyDXBase
{




    struct VHG_Instance
    {
        DirectX::XMFLOAT3       inst_pos;
        DirectX::XMFLOAT3       inst_attributes;
    };





    struct VHG_Vertex_PosTex
    {
        DirectX::XMFLOAT3       e_pos;
        DirectX::XMFLOAT2       e_texco;
    };







    template <class T>
    class VHG_Scale
    {
    public:
        VHG_Scale(float p_scale_factor) : e_scale_factor(p_scale_factor) {}


        void posApply(std::vector<T> & p_vectpostex)
        {
            for_each(
                p_vectpostex.begin(),
                p_vectpostex.end(),

                //      Important : pass p_postex BY REFERENCE 
                //      otherwise won't be able to alter it!!! 

                [this](T & p_postex) {
                    p_postex.e_pos.x *= this->e_scale_factor;
                    p_postex.e_pos.y *= this->e_scale_factor;
                    p_postex.e_pos.z *= this->e_scale_factor;
                }
            );
        }

    private:
        float e_scale_factor;
    };










	class HvyInst
	{
	public:


		HvyInst(
            std::shared_ptr<DX::DeviceResources>    const &     deviceResources, 
            Microsoft::WRL::ComPtr<ID3D11Buffer>    const &     MVP_Buffer
        );


        void                        CreateDeviceDepResources();


        void                        CreateRasterizerSolid();
        void                        CreateRasterizerWireframe();

        void                        CreateSamplerStateGeneric();

        void                        DefineTheProtoCube();


        void                        TriflecheCreateInstanceBuffer();


        uint32_t                    TriflecheMesh(std::vector<VHG_Instance>    *p_vect_Instances);


        void                        LerpFlechette(std::vector<HvyDXBase::VHG_Instance>    *p_vect_Instances, DirectX::XMVECTOR const &     p_endPointXMV, uint32_t idx);


        void                        TriflecheRender();




        void LoxodromeBeta(std::vector<VHG_Instance>    *p_vect_Instances, double beta); 





	public:


        bool                                                        m_loadingComplete;






	private:

		std::shared_ptr<DX::DeviceResources>                        m_deviceResources;

        ID3D11Buffer* const *                                       gv_fail; 











        Microsoft::WRL::ComPtr<ID3D11InputLayout>            ProtoCube_InputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                 ProtoCube_VertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                 ProtoCube_IndexBuffer;
        uint32_t                                             ProtoCube_IndexCount;





        Microsoft::WRL::ComPtr<ID3D11Buffer>                    TriflecheInstanceBuffer;
        uint32_t                                                TriflecheInstanceCount;


        Microsoft::WRL::ComPtr<ID3D11Buffer>                    S2ResultInstanceBuffer;
        uint32_t                                                S2ResultInstanceCount;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                    S2ConstraintsInstanceBuffer;
        uint32_t                                                S2ConstraintsInstanceCount;




        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>        e_srv_axis_x;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>        e_srv_axis_y;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>        e_srv_axis_z;



        Microsoft::WRL::ComPtr<ID3D11VertexShader>              instance_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>               instance_PixelShader;

        Microsoft::WRL::ComPtr<ID3D11SamplerState>				e_SamplerStateGeneric;

        Microsoft::WRL::ComPtr<ID3D11RasterizerState>           e_RasterizerStateWireframe;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState>           e_RasterizerStateSolid;










	};
}




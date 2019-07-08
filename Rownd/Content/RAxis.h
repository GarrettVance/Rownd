#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"






namespace HvyDXBase
{













    class RAxis
    {
    public:


        RAxis(
            std::shared_ptr<DX::DeviceResources>    const &     deviceResources, 
            Microsoft::WRL::ComPtr<ID3D11Buffer>    const &     MVP_Buffer
        );


        void                        CreateDeviceDepResources();


        void                        CreateRasterizerSolid();
        void                        CreateRasterizerWireframe();

        void                        CreateSamplerStateGeneric();

        void                        DefineTheProtoCube();


        void                        TriflecheCreateInstanceBuffer();


        uint32_t                    TriflecheMesh(std::vector<VHG_Instance>    *p_vect_Instances,
            DirectX::XMVECTOR const & p_axisvector);


        void                        LerpFlechette(std::vector<HvyDXBase::VHG_Instance>    *p_vect_Instances, DirectX::XMVECTOR const &     p_endPointXMV, uint32_t idx);


        void                        TriflecheRender();


        void                        UpdateConnector(DirectX::XMVECTOR const &  p_axisvector); 







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


        // Microsoft::WRL::ComPtr<ID3D11Buffer>                    S2ResultInstanceBuffer;
        // uint32_t                                                S2ResultInstanceCount;

        // Microsoft::WRL::ComPtr<ID3D11Buffer>                    S2ConstraintsInstanceBuffer;
        // uint32_t                                                S2ConstraintsInstanceCount;




        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>        e_srv_axis_x;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>        e_srv_axis_y;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>        e_srv_axis_z;



        Microsoft::WRL::ComPtr<ID3D11VertexShader>              instance_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>               instance_PixelShader;

        Microsoft::WRL::ComPtr<ID3D11SamplerState>              e_SamplerStateGeneric;

        Microsoft::WRL::ComPtr<ID3D11RasterizerState>           e_RasterizerStateWireframe;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState>           e_RasterizerStateSolid;










    };
}




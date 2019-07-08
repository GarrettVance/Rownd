

#pragma once


#include <wrl.h>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <memory>
#include <agile.h>
#include <concrt.h>



#include <vector>


#include <complex>


#define GHV_OPTION_LOCAL_TEXTURE_COORDINATES    //  keep this defined. The switch for local/global is inside vertex shader.




#undef GHV_OPTION_SHOW_PRETTY_HYPERBOLIC



#undef GHV_OPTION_DEXTRAL


#define GHV_OPTION_LOXODROME


#undef GHV_OPTION_STEREOGRAPHIC_LOWER_HEMISPHERE



#pragma comment(lib, "DirectXTK")

#include "Keyboard.h"
#include "Mouse.h"

#include "SimpleMath.h"


#include "WICTextureLoader.h" 


#include "DDSTextureLoader.h" 




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





}


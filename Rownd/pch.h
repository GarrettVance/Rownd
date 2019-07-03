

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





#define GHV_OPTION_LOCAL_TEXTURE_COORDINATES    //  keep this defined. The switch for local/global is inside vertex shader.




#undef GHV_OPTION_SHOW_PRETTY_HYPERBOLIC



#undef GHV_OPTION_DEXTRAL


#define GHV_OPTION_LOXODROME


#undef GHV_OPTION_STEREOGRAPHIC_LOWER_HEMISPHERE



#pragma comment(lib, "DirectXTK")

#include "Keyboard.h"
#include "Mouse.h"


#include "WICTextureLoader.h" 


#include "DDSTextureLoader.h" 





//               
//              
//                     ghv Instancing Pixel Shader  
//     




Texture2D xAxisTexture      :   register(t0);
Texture2D yAxisTexture      :   register(t1);
Texture2D zAxisTexture      :   register(t2);

SamplerState ObjSamplerState;





struct PixelShaderInput
{
	float4     pos         : SV_POSITION;
	float2     texco       : TEXCOORD0;
    float4     anim_color  : COLOR; 
    uint       prim_id     : SV_PrimitiveID; 
};








float4       main(PixelShaderInput      input) : SV_TARGET
{
    float4 v_rgba; 

    if (input.anim_color.x > 300.0)
    {
        v_rgba = zAxisTexture.Sample(ObjSamplerState, input.texco);
    }
    else if (input.anim_color.x > 200.0)
    {
        v_rgba = yAxisTexture.Sample(ObjSamplerState, input.texco);
    }
    else
    {
        v_rgba = xAxisTexture.Sample(ObjSamplerState, input.texco);
    }

    return v_rgba;
}





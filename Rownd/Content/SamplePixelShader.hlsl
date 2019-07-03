



Texture2D           ColorTexture        :       register(t0);

SamplerState        LinearSampler       :       register(s0);






struct PixelShaderInput
{
	float4 pos          : SV_POSITION;
	float3 color        : COLOR0;
    float2 texco        : TEXCOORD;
};






float4 main(PixelShaderInput input) : SV_TARGET
{

    /*
    float c_red = input.texco.x; 
    float c_green = 0.3;
    float c_blue = input.texco.y;
    float4 color4 = float4(c_red, c_green, c_blue, 1.0); 
    return color4;
    */


    float4  SurfaceColor = ColorTexture.Sample(LinearSampler, input.texco);

    // if (SurfaceColor.w > 0.6)
    // {
    //     SurfaceColor = float4(0.f, 0.f, 0.f, 0.6f); 
    // }

    return SurfaceColor;

}

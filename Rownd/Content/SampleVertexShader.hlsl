

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};


struct VertexShaderInput
{
	float3 pos          : POSITION;
    float3 nor          : NORMAL;
    float2 texGlobal    : TEXCOORD0;
    float2 texLocal     : TEXCOORD1;
};


struct PixelShaderInput
{
	float4 pos          : SV_POSITION;
	float3 color        : COLOR0;
    float2 texco        : TEXCOORD;
};



PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);

	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;


    output.color = float3(1.0, 1.0, 1.0); 

#if 3 == 3
    //  Use local texture coordinates

    output.texco = input.texLocal; 

#else

    // undo: 
    output.texco = input.texGlobal; 


    /*
    float su = input.nor.x / 2.0 + 0.5;
    float sv = input.nor.y / 2.0 + 0.5;
    output.texco = float2(su, sv); 
    */

#endif

	return output;
}



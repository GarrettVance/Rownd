//         
// 
//                 ghv Instancing Vertex Shader 
// 
            


cbuffer WVP_ConstantBufferStruct : register(b0)
{
	matrix   model;
	matrix   view;
	matrix   projection;
};



struct VertexShaderInput
{
	float3    pos                   : POSITION;
	float2    texco                 : TEXCOORD0; 
    float3    inst_pos              : TEXCOORD1; 
    float3    inst_attributes       : TEXCOORD2; 
    uint      inst_id               : SV_InstanceID; 
    uint       vert_id              : SV_VertexID;
};



struct PixelShaderInput
{
	float4     pos         : SV_POSITION;
	float2     texco       : TEXCOORD0;
    float4     anim_color  : COLOR; 
};



PixelShaderInput main(VertexShaderInput input)
{
     PixelShaderInput output;
     
     float4 vertex_location = float4(input.pos, 1.0f);


     vertex_location.x += input.inst_pos.x;
     vertex_location.y += input.inst_pos.y;
     vertex_location.z += input.inst_pos.z;


    // Transform "vertex_location" into projected space.
     
    vertex_location = mul(vertex_location, model);
    vertex_location = mul(vertex_location, view);
    vertex_location = mul(vertex_location, projection);
    output.pos = vertex_location;
     
     
    output.texco = input.texco;


    float4 tmp_anim_color = float4(0.f, 0.f, 0.f, 0.f); 
    tmp_anim_color.x = (float)input.inst_attributes.x;
    tmp_anim_color.y = (float)input.inst_id;
    tmp_anim_color.z = (float)input.vert_id;
    output.anim_color = tmp_anim_color;



     return output;
}





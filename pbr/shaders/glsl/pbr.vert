#version 450 core

// 基于物理的着色模型：顶点程序
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec2 texcoord;

// 统一缓冲对象，用于变换矩阵
layout(std140, binding=0) uniform TransformUniforms
{
	mat4 viewProjectionMatrix;		// 视图投影矩阵
	mat4 skyProjectionMatrix;		// 天空投影矩阵
	mat4 sceneRotationMatrix;		// 场景旋转矩阵
};

// 输出顶点数据
layout(location=0) out Vertex
{
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;		 // 输出切线基（用于法线贴图）
} vout;

uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;

void main()
{
	// 计算变换后的顶点位置（不包括投影变换）
	vout.position = vec3(sceneRotationMatrix * vec4(position, 1.0));

	// 翻转纹理坐标的Y轴
	vout.texcoord = vec2(texcoord.x, 1.0-texcoord.y);

	// 计算切线空间基向量（用于法线贴图）
	vout.tangentBasis = mat3(sceneRotationMatrix) * mat3(tangent, bitangent, normal);

	// 计算顶点的最终位置，包括视图投影变换
	gl_Position = viewProjectionMatrix * sceneRotationMatrix *model* vec4(position, 1.0);
}

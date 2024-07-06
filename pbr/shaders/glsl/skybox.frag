#version 450 core

// 环境天空盒：片段着色器。
layout(location=0) in vec3 localPosition;				// 输入属性
layout(location=0) out vec4 color;						// 输出颜色
layout(binding=0) uniform samplerCube envTexture;		// 环境贴图采样器

void main()
{
    // 标准化环境向量
    vec3 envVector = normalize(localPosition);
    // 从环境贴图中获取颜色
    color = textureLod(envTexture, envVector, 0);
}

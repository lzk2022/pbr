#version 450 core
// 环境天空盒：顶点程序。

// 统一缓冲绑定0中的变换统一变量
layout(std140, binding=0) uniform TransformUniforms
{
    mat4 viewProjectionMatrix;   // 视图投影矩阵
    mat4 skyProjectionMatrix;    // 天空投影矩阵
    mat4 sceneRotationMatrix;    // 场景旋转矩阵
};

layout(location=0) in vec3 position;
layout(location=0) out vec3 localPosition;

void main()
{
    // 将本地位置设置为顶点坐标的XYZ分量
    localPosition = position.xyz;
    // 计算顶点在天空盒中的位置
    gl_Position = skyProjectionMatrix * vec4(position, 1.0);
}

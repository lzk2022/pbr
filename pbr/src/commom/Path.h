#include "Log.h"

#define PATH "../resource/"

//#define PATH "C:\\work\\vs 2022\\pbr02\\data\\"

const int gDisplaySizeX = 1920;         // 显示窗口的宽度，单位为像素
const int gDisplaySizeY = 1080;         // 显示窗口的高度，单位为像素
const int gDisplaySamples = 16;			// 渲染时的采样数量，越高质量越好，但也会增加计算量
const float gViewDistance = 150.0f;     // 视点与目标之间的距离
const float gViewFOV = 45.0f;           // 视野的角度大小，单位为度（degree）
const float gOrbitSpeed = 1.0f;         // 轨道旋转速度，影响视点围绕目标旋转的速度
const float gZoomSpeed = 4.0f;          // 缩放速度，影响视点拉近和远离目标的速度

// Parameters
static constexpr int gEnvMapSize = 1024;		// 环境贴图的大小（用于反射和光照计算）
static constexpr int gIrradianceMapSize = 32;	// 辐照度贴图的大小（用于漫反射光照计算）
static constexpr int gBRDF_LUT_Size = 256;		// BRDF 查找表的大小（用于镜面反射计算）

// 各向异性过滤（Anisotropic Filtering，简称AF）是一种提高纹理过滤质量的技术，特别是在纹理与观察
// 者视角成锐角时。maxAnisotropy通常表示图形硬件支持的最大各向异性过滤级别。例如，值为16表示硬件
// 支持的最大各向异性过滤级别是16倍。

extern float gMaxAnisotropy;					// 最大各向异性

void InitPath();





# 基于物理的渲染

一种在OpenGL和 Vulkan API中实现基于物理的着色模型和基于图像的照明的方法。

未使用灯光的情况
![alt text](image/README/image-1.png)

F1 使用灯光1的情况
![alt text](image/README/image-2.png)

F2 使用灯光2的情况
![alt text](image/README/image-3.png)


| API         |  实现状态 |
|-------------|----------|
| OpenGL 4.5  |  完成     |
| Vulkan      |  进行中   |


## 关于

我们的Physically Based Rendering (PBR)项目展示了如何使用OpenGL进行现代图形技术集成，重点介绍了应用PBR原理以实现逼真渲染的过程。PBR基于物理模型，精确模拟光线与材质相互作用的行为，从而产生更加真实的视觉效果。
后面会添加Vulkan的实现方式

着色器中有大量注释，因为那里是有趣的事情发生的地方。:)

## 构建

### Windows

#### 先决条件

- Windows 10 或 Windows 11 (x64 版本)
- Visual Studio 2022 (任何版本)
- 相对较新的 Windows 10 SDK 版本
- [LunarG Vulkan SDK](https://vulkan.lunarg.com/sdk/home)

#### 如何构建

Visual Studio 解决方案可在```pbr/pbr.sln```中找到。成功构建后，生成的可执行文件和所有需要的DLL可以在```plugs/dll```目录中找到。请注意，预编译的第三方库仅适用于x64目标。

### 控制

| 输入       | 动作          |
|------------|---------------|
| LMB 拖动   | 旋转相机      |
| RMB 拖动   | 旋转3D模型    |
| 滚轮       | 放大/缩小     |
| F1-F3      | 切换分析灯光开/关 |

## 参考文献

该基于物理的着色实现主要基于以下课程获得的信息：

- [Real Shading in Unreal Engine 4](http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf)，Brian Karis，SIGGRAPH 2013
- [Moving Frostbite to Physically Based Rendering](https://seblagarde.wordpress.com/2015/07/14/siggraph-2014-moving-frostbite-to-physically-based-rendering/)，Sébastien Lagarde，Charles de Rousiers，SIGGRAPH 2014

其他在研究和实现过程中帮助我的资源：

- [Adopting Physically Based Shading Model](https://seblagarde.wordpress.com/2011/08/17/hello-world/)，Sébastien Lagarde
- [Microfacet Models for Refraction through Rough Surfaces](https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf)，Bruce Walter 等人，Eurographics，2007
- [An Inexpensive BRDF Model for Physically-Based Rendering](http://igorsklyar.com/system/documents/papers/28/Schlick94.pdf)，Christophe Schlick，Eurographics，1994
- [GPU-Based Importance Sampling](https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch20.html)，Mark Colbert，Jaroslav Křivánek，GPU Gems 3，2007
- [Hammersley Points on the Hemisphere](http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html)，Holger Dammertz
- [Notes on Importance Sampling](http://blog.tobias-franke.eu/2014/03/30/notes_on_importance_sampling.html)，Tobias Franke
- [Specular BRDF Reference](http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html)，Brian Karis
- [To PI or not to PI in game lighting equation](https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/)，Sébastien Lagarde
- [Physically Based Rendering: From Theory to Implementation, 2nd ed.](https://www.amazon.com/Physically-Based-Rendering-Second-Implementation/dp/0123750792)，Matt Pharr，Greg Humphreys，2010
- [Advanced Global Illumination, 2nd ed.](https://www.amazon.com/Advanced-Global-Illumination-Second-Philip/dp/1568813074)，Philip Dutré，Kavita Bala，Philippe Bekaert，2006
- [Photographic Tone Reproduction for Digital Images](https://www.cs.utah.edu/~reinhard/cdrom/)，Erik Reinhard 等人，2002

## 第三方库

该项目使用了以下开源库：

- [Open Asset Import Library](http://assimp.sourceforge.net/)
- [stb_image](https://github.com/nothings/stb)
- [GLFW](http://www.glfw.org/)
- [GLM](https://glm.g-truc.net/)
- [glad](https://github.com/Dav1dde/glad) (用于生成OpenGL函数加载器)
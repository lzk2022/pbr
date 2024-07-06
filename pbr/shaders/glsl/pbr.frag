#version 450 core


// 物理基础着色模型：朗伯漫反射BRDF + Cook-Torrance微平面镜面反射BRDF + 环境光照映射（IBL）用于环境光。

// 这个实现基于 Epic Games 在 SIGGRAPH 2013 的课程笔记 "Real Shading in Unreal Engine 4"。
// 参考链接：http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf


const float PI = 3.141592;
const float Epsilon = 0.00001;

const int NumLights = 3;
const vec3 Fdielectric = vec3(0.04);		// 所有介质的常数法线入射菲涅尔因子

struct AnalyticalLight {
	vec3 direction;		// 光源的方向向量
	vec3 radiance;		// 光源的辐射能量
};

layout(location=0) in Vertex
{
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;
} vin;

layout(location=0) out vec4 color;

layout(std140, binding=1) uniform ShadingUniforms
{
	AnalyticalLight lights[NumLights];
	vec3 eyePosition;
};

layout(binding=0) uniform sampler2D albedoTexture;					// 漫反射贴图
layout(binding=1) uniform sampler2D normalTexture;					// 法线贴图
layout(binding=2) uniform sampler2D metalnessTexture;				// 金属度贴图
layout(binding=3) uniform sampler2D roughnessTexture;				// 粗糙度贴图
layout(binding=4) uniform samplerCube specularTexture;				// 镜面反射贴图
layout(binding=5) uniform samplerCube irradianceTexture;			// 照射度贴图
layout(binding=6) uniform sampler2D specularBRDF_LUT;				// 镜面BRDF查找表贴图

// GGX/Towbridge-Reitz法线分布函数
// 使用 Disney 的重新参数化，alpha = roughness^2
float ndfGGX(float cosLh, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// 可分离Schlick-GGX的单项函数
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// 使用Smith方法的Schlick-GGX几何衰减函数的Schlick近似
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; 		// Epic建议使用这种粗糙度重映射来解析光源
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick的菲涅尔因子近似
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	// 样本输入纹理以获取着色模型参数
	vec3 albedo = texture(albedoTexture, vin.texcoord).rgb;
	float metalness = texture(metalnessTexture, vin.texcoord).r;
	float roughness = texture(roughnessTexture, vin.texcoord).r;

	// 出射光方向（从世界空间片段位置到“眼睛”的向量）
	vec3 Lo = normalize(eyePosition - vin.position);

	// 获取当前片段的法线并转换到世界空间
	vec3 N = normalize(2.0 * texture(normalTexture, vin.texcoord).rgb - 1.0);
	N = normalize(vin.tangentBasis * N);
	
	// 表面法线与出射光方向之间的角度
	float cosLo = max(0.0, dot(N, Lo));
		
	// 镜面反射向量
	vec3 Lr = 2.0 * cosLo * N - Lo;

	// 法线入射处的菲涅尔反射率（对于金属使用反射率颜色）
	vec3 F0 = mix(Fdielectric, albedo, metalness);

	// 分析光源的直接光照计算
	vec3 directLighting = vec3(0);
	for(int i=0; i<NumLights; ++i)
	{
		vec3 Li = -lights[i].direction;
		vec3 Lradiance = lights[i].radiance;

		// Li和Lo之间的半程向量
		vec3 Lh = normalize(Li + Lo);

		// 计算表面法线和各种光向量之间的角度
		float cosLi = max(0.0, dot(N, Li));
		float cosLh = max(0.0, dot(N, Lh));

		// 计算直接光照的菲涅尔项
		vec3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
		 // 计算镜面BRDF的法线分布
		float D = ndfGGX(cosLh, roughness);
		// 计算镜面BRDF的几何衰减
		float G = gaSchlickGGX(cosLi, cosLo, roughness);

		// 漫反射是由介质媒介中的光被折射多次而发生的
        // 金属反之要么反射要么吸收能量，所以漫反射贡献始终为零
        // 为了能量守恒，我们必须根据菲涅尔因子和金属度来缩放漫反射BRDF的贡献
		vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

        // Lambert漫反射BRDF。
        // 我们不按1/PI缩放光照和材质单位，以便更方便
        // 参考：https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
		vec3 diffuseBRDF = kd * albedo;

		// Cook-Torrance 镜面微表面 BRDF.
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

		// 该光源的总贡献
		directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
	}

	// 环境光照（IBL）
	vec3 ambientLighting;
	{
		// 在法线方向采样漫反射辐照度
		vec3 irradiance = texture(irradianceTexture, N).rgb;

		// 计算环境光照的菲涅尔项。
		// 由于我们使用预过滤的立方体贴图(s)和辐照度来自多个方向，
		// 使用cosLo而不是与光的半向量的角度（上面的cosLh）。
		// 参考：https://seblagarde.wordpress.com/2011/08/17/hello-world/
		vec3 F = fresnelSchlick(F0, cosLo);

		// 获取漫反射贡献因子（类似于直接光照中的计算）。
		vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

		// 辐照度贴图假设朗伯BRDF，这里不需要乘以1/PI来缩放。
		vec3 diffuseIBL = kd * albedo * irradiance;

		// 在正确的mipmap级别采样预过滤的镜面反射环境
		int specularTextureLevels = textureQueryLevels(specularTexture);
		vec3 specularIrradiance = textureLod(specularTexture, Lr, roughness * specularTextureLevels).rgb;

		// Cook-Torrance镜面BRDF的分割求和近似因子
		vec2 specularBRDF = texture(specularBRDF_LUT, vec2(cosLo, roughness)).rg;

		// 总的镜面IBL贡献
		vec3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

		// 总的环境光照贡献
		ambientLighting = diffuseIBL + specularIBL;
	}

	// 最终片段颜色
	color = vec4(directLighting + ambientLighting, 1.0);
}

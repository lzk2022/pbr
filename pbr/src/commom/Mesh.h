#pragma once
#ifndef __MESH_H__
#define __MESH_H__

#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

class Mesh
{
public:
	struct Vertex{				// 顶点数据
		glm::vec3 position;		// 顶点位置
		glm::vec3 normal;		// 顶点法线
		glm::vec3 tangent;		// 顶点切线
		glm::vec3 bitangent;	// 顶点副切线
		glm::vec2 texcoord;		// 纹理坐标
	};
	static_assert(sizeof(Vertex) == 14 * sizeof(float));
	static const int mkNumAttributes = 5;

	struct Triangle{				// 三角形
		uint32_t v1, v2, v3;		// 三个顶点
	};
	static_assert(sizeof(Triangle) == 3 * sizeof(uint32_t));

	static std::shared_ptr<Mesh> ReadFile(const std::string& filename);
	static std::shared_ptr<Mesh> ReadString(const std::string& data);

private:
	Mesh(const struct aiMesh* mesh);

public:
	std::vector<Vertex> mVertices;
	std::vector<Triangle> mTriangle;
};

#endif // !__MESH_H__
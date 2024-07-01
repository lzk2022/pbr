#include <cstdio>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "Mesh.h"
#include "Path.h"

namespace {
	const unsigned int ImportFlags = 
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_PreTransformVertices |
		aiProcess_GenNormals |
		aiProcess_GenUVCoords |
		aiProcess_OptimizeMeshes |
		aiProcess_Debone |
		aiProcess_ValidateDataStructure;
}

struct LogStream : public Assimp::LogStream
{
	static void Init()
	{
		if(Assimp::DefaultLogger::isNullLogger()) {
			Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
			Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
		}
	}
	// ¼Ì³ÐÓÚ¸¸º¯Êý
	void write(const char* message) override
	{
		std::fprintf(stderr, "Assimp: %s", message);
	}
};

Mesh::Mesh(const aiMesh* mesh)
{
	assert(mesh->HasPositions());
	assert(mesh->HasNormals());

	mVertices.reserve(mesh->mNumVertices);
	for(size_t i=0; i<mVertices.capacity(); ++i) 
	{
		Vertex vertex;
		vertex.position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
		vertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
		if(mesh->HasTangentsAndBitangents()) 
		{
			vertex.tangent = {mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
			vertex.bitangent = {mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
		}
		if(mesh->HasTextureCoords(0)) 
		{
			vertex.texcoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
		}
		mVertices.push_back(vertex);
	}
	
	mTriangle.reserve(mesh->mNumFaces);
	for(size_t i=0; i<mTriangle.capacity(); ++i) 
	{
		assert(mesh->mFaces[i].mNumIndices == 3);
		mTriangle.push_back({mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2]});
	}
}

std::shared_ptr<Mesh> Mesh::ReadFile(const std::string& filename1)
{
	std::string filename = PATH + filename1;
	LogStream::Init();

	LOG_INFO("Loading mesh: " + filename1);
	
	std::shared_ptr<Mesh> mesh;
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename, ImportFlags);
	LOG_ASSERT(!(scene && scene->HasMeshes()), "Failed to load mesh file: " + filename1);
	mesh = std::shared_ptr<Mesh>(new Mesh{ scene->mMeshes[0] });
	return mesh;
}

std::shared_ptr<Mesh> Mesh::ReadString(const std::string& data)
{
	LogStream::Init();

	std::shared_ptr<Mesh> mesh;
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFileFromMemory(data.c_str(), data.length(), ImportFlags, "nff");
	LOG_ASSERT(!(scene && scene->HasMeshes()), "Failed to create mesh from string: " + data);
	mesh = std::shared_ptr<Mesh>(new Mesh{ scene->mMeshes[0] });
	return mesh;
}

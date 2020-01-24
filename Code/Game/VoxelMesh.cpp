#include "VoxelMesh.hpp"
#include "Engine/Render/Utils/CPUMesh.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <fstream>

void VoxelMesh::AddVoxel(const Vec3& pos, const Rgba& color)
{
	m_voxels.push_back(VoxelGrid{ pos, color });
}

void VoxelMesh::AddVoxel(const VoxelGrid& voxel)
{
	m_voxels.push_back(voxel);
}

void VoxelMesh::SetScale(float scale)
{
	m_scale = scale;
}

void VoxelMesh::LoadFromFiles(std::string filepath)
{
	std::ifstream objFile;
	objFile.open(filepath);
	Vec3 pos;
	Rgba color;

	std::string curLine;

	// Skip useless lines
	for(int i = 0; i < 11; i++)
	{
		getline(objFile, curLine);
	}

	//do
	//{
	//	getline(objFile, curLine);
	//}
	//while(curLine != "end_header");

	// Load points
	while(getline(objFile,curLine))
	{
		std::vector<std::string> elements;
		SplitStringByPattern(curLine, elements, " ");

		pos.x = (float)atof(elements[0].c_str());
		pos.y = (float)atof(elements[1].c_str());
		pos.z = (float)atof(elements[2].c_str());

		color.r = (float)atof(elements[3].c_str()) / 255.f;
		color.g = (float)atof(elements[4].c_str()) / 255.f;
		color.b = (float)atof(elements[5].c_str()) / 255.f;
		color.a = 1.f;

		AddVoxel(pos, color);
	}

	objFile.close();
}

CPUMesh* VoxelMesh::GenerateMesh()
{
	CPUMesh* voxelMesh = new CPUMesh();

	for(VoxelGrid v: m_voxels)
	{
		AABB3 box = AABB3(v.pos - Vec3(0.5f, 0.5f, 0.5f), v.pos + Vec3(0.5f, 0.5f, 0.5f));
		CPUMeshAddCube(voxelMesh, box, v.Color);
	}

	return voxelMesh;
}
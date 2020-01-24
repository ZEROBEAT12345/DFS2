#pragma once
#include <vector>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Matrix44.hpp"

class CPUMesh;

struct VoxelGrid
{
	Vec3 pos;
	Rgba Color;
};

class VoxelMesh
{
public:

	VoxelMesh() {}
	~VoxelMesh() {}

	void AddVoxel(const Vec3& pos, const Rgba& color);
	void AddVoxel(const VoxelGrid& voxel);
	void SetScale(float scale);
	void LoadFromFiles(std::string filepath);
	CPUMesh* GenerateMesh();

private:
	Matrix44 m_transform;
	float m_scale;
	std::vector<VoxelGrid> m_voxels;
};
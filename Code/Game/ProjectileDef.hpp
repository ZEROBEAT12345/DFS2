#pragma once
#include <string>

class ProjectileDef
{
public:
	ProjectileDef() {}
	~ProjectileDef(){}

public:
	float velocity = 40.f;
	float existTime = 6.f;
	float damageCoef = 10.f;
	float collisionRadius = 5.f;
	float size = 1.f;
	std::string meshName = "Apple";
};
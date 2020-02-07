#pragma once
#include <string>

class ProjectileDef
{
public:
	ProjectileDef() {}
	~ProjectileDef(){}

public:
	float velocity = 10.f;
	float existTime = 5.f;
	float damageCoef = 1.f;
	float collisionRadius = 5.f;
	std::string meshName = "Apple";
};
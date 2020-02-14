#pragma once
#include <string>

class ProjectileDef
{
public:
	ProjectileDef() {}
	~ProjectileDef(){}

public:
	float velocity = 20.f;
	float existTime = 6.f;
	float damageCoef = 10.f;
	float collisionRadius = 5.f;
	std::string meshName = "Apple";
};
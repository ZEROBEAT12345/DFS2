#include "Game/SkillDefinition.hpp"
#include "Game/ProjectileDef.hpp"
#include "Game/Projectile.hpp"
#include "Game/PlayerController.hpp"
#include "Game/Map.hpp"

void SkillDefinition::Cast(PlayerController* player, Map* curMap)
{
	Projectile* newP = new Projectile(m_pDef,player->GetPos(), player->GetForwardAngle(), 14.f, player->GetPlayerID());
	newP->Initialize();
	curMap->AddProjectile(newP);
}
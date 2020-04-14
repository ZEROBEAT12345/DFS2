#include "Game/SkillDefinition.hpp"
#include "Game/ProjectileDef.hpp"
#include "Game/Projectile.hpp"
#include "Game/PlayerController.hpp"
#include "Game/Map.hpp"
#include "Engine/Math/Algorithms/MathUtils.hpp"

void SkillDefinition::Cast(PlayerController* player, Map* curMap)
{
	// Hardcoded

	switch(m_skillType)
	{
	case SKILL_NEWTON_NORMAL_ATTACK:
		// Spawn new projectile
		if (m_pDef)
		{
			Projectile* newP = new Projectile(m_pDef, curMap, player->GetPos() + curMap->GetGridScale() * .3f *
				Vec2(CosDegrees(player->GetForwardAngle()), SinDegrees(player->GetForwardAngle())), player->GetForwardAngle(), 14.f, player->GetPlayerID());
			newP->Initialize();
			curMap->AddProjectile(newP);
		}
		break;
	case SKILL_NEWTON_SKILL_1:
		// Spawn new projectile
		if (m_pDef)
		{
			Projectile* newP = new Projectile(m_pDef, curMap, player->GetPos() + curMap->GetGridScale() * .3f *
				Vec2(CosDegrees(player->GetForwardAngle()), SinDegrees(player->GetForwardAngle())), player->GetForwardAngle(), 14.f, player->GetPlayerID(), PROJECTILE_NEWTON_SKILL_1);
			newP->Initialize();
			curMap->AddProjectile(newP);
		}
		break;
	case SKILL_NEWTON_SKILL_2:
		break;
	case SKILL_JONES_NORMAL_ATTACK:
		break;
	case SKILL_JONES_SKILL_1:
		break;
	case SKILL_JONES_SKILL_2:
		break;
	}


}
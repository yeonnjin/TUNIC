#pragma once

#include "Base.h"

BEGIN(Engine)

class CCollision_Manager final : public CBase
{
public:
	enum GROUP {
		GROUP_PLAYER, GROUP_PLAYER_WEAPON, GROUP_MONSTER, GROUP_MONSTER_WEAPON, GROUP_INTERACTIVE,
		GROUP_OBSTACLE, GROUP_END
	};

private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	HRESULT Add_Group(GROUP eCollisionGroup, class CGameObject* pGameObject);
	HRESULT Add_RigidGroup(class CGameObject* pGameObject);
	HRESULT Clear_Group();
	void	Check_Collision_Groups(GROUP eCollisionGroupA, GROUP eCollisionGroupB);
	void	Check_Rigid_Groups();

public:
	HRESULT	Initialize();

private:
	array<vector<class CGameObject*>, GROUP_END>	m_arrCollisionGroups;
	vector<class CGameObject*>						m_RigidGroups;

public:
	static CCollision_Manager* Create();
	virtual void Free() override;
};

END
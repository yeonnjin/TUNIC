#pragma once

#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct GameObject_Desc : public CTransform::TRANSFORM_DESC
	{		
	}GAMEOBJECT_DESC;

public:
	enum OBJECT { OBJ_PLAYER, OBJ_PLAYER_WEAPON, OBJ_MONSTER, OBJ_END };

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	// Set
	void				Set_HP(_int iHP) { m_iHP += iHP; /*if (0 > m_iHP) m_iHP = 0;*/ }
	void				Set_Dead(_bool isDead) { m_isDead = isDead; }
	void				Set_isDamage(_bool isDamage) { m_isDamage = isDamage; }

	// Get
	class CComponent*	Get_Component(const wstring& strComTag);
	OBJECT				Get_ObjectType() { return m_eType; }
	_uint				Get_HP() { return m_iHP; }
	_bool				Get_isDamage() { return m_isDamage; }

	// Collision
	virtual void		Collision_Event(CGameObject* pGameObject) {}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT	Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };

	class CGameInstance*				m_pGameInstance = { nullptr };
	CTransform*							m_pTransformCom = { nullptr };

protected:
	map<const wstring, class CComponent*>		m_Components;
	OBJECT										m_eType = { OBJ_END };

	_int										m_iHP = { 5 };
	_bool										m_isDamage = { false };
	_bool										m_isDead = { false };
	_float										m_fDamageCoolTime = { 0.f };

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, class CComponent** ppOut, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
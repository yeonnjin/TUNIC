#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "Monster_CowBot.h"

BEGIN(Engine)
class CBone;
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CCowBot_Weapon final : public CPartObject
{
public:
	typedef struct CowBot_Weapon_Desc : public CPartObject::PARTOBJECT_DESC
	{
		_uint						m_iLevel;
		CBone*						pSocketBone = { nullptr };
		wstring						strModelComTag;
		CMonster_CowBot::WEAPON		eWeapon;
	}COWBOT_WEAPON_DESC;

private:
	CCowBot_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCowBot_Weapon(const CCowBot_Weapon& rhs);
	virtual ~CCowBot_Weapon() = default;

public:
	void			Set_isRender(_bool isRender) { m_isRender = isRender; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	wstring						m_strModelComTag = {};
	CMonster_CowBot::WEAPON		m_eWeapon = {};
	_bool						m_isRender = { true };

private:
	CModel*						m_pModelCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	CBone*						m_pSocketBone = { nullptr };
	CCollider*					m_pColliderCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CCowBot_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CPartObject을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END
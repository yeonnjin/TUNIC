#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "Monster_Guard.h"

BEGIN(Engine)
class CBone;
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CGuard_Weapon_Spear final : public CPartObject
{
public:
	typedef struct Guard_Weapon_Spear_Desc : public CPartObject::PARTOBJECT_DESC
	{
		_uint						m_iLevel;
		CBone*						pSocketBone = { nullptr };
		wstring						strModelComTag;
		CMonster_Guard::WEAPON		eWeapon;
	}GUARD_WEAPON_SPEAR_DESC;

private:
	CGuard_Weapon_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGuard_Weapon_Spear(const CGuard_Weapon_Spear& rhs);
	virtual ~CGuard_Weapon_Spear() = default;

public:
	void						Set_isRender(_bool isRender) { m_isRender = isRender; }
	void						Set_isAttackFrame(_bool isAttack) { m_isAttackFrame = isAttack; }

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual HRESULT				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT				Render_LightDepth() override;

private:
	wstring						m_strModelComTag = {};
	CMonster_Guard::WEAPON		m_eWeapon = {};
	_bool						m_isRender = { true };
	_bool						m_isAttackFrame = { false };

private:
	CModel*						m_pModelCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	CBone*						m_pSocketBone = { nullptr };
	CCollider*					m_pColliderCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CGuard_Weapon_Spear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CPartObject을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END
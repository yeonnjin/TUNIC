#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "Monster_Frog.h"

BEGIN(Engine)
class CBone;
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CFrog_Weapon final : public CPartObject
{
public:
	typedef struct Frog_Weapon_Desc : public CPartObject::PARTOBJECT_DESC
	{
		_uint						m_iLevel;
		CBone*						pSocketBone = { nullptr };
		wstring						strModelComTag;
		CMonster_Frog::WEAPON		eWeapon;
	}FROG_WEAPON_DESC;

private:
	CFrog_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFrog_Weapon(const CFrog_Weapon& rhs);
	virtual ~CFrog_Weapon() = default;

public:
	void			Set_isRender(_bool isRender) { m_isRender = isRender; }
	void			Set_isAttackFrame(_bool isAttack) { m_isAttackFrame = isAttack; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	wstring						m_strModelComTag = {};
	CMonster_Frog::WEAPON		m_eWeapon = {};
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
	static CFrog_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CPartObject을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END
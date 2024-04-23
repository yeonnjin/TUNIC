#pragma once

#include "Client_Defines.h"
#include "PartObject.h"
#include "Player.h"

BEGIN(Engine)
class CBone;
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CPlayer_Weapon abstract : public CPartObject
{
public:
	typedef struct Player_Weapon_Desc : public CPartObject::PARTOBJECT_DESC
	{
		CBone* pSocketBone = { nullptr };
		CPlayer::WEAPON		eWeapon;
	}PLAYER_WEAPON_DESC;

protected:
	CPlayer_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Weapon(const CPlayer_Weapon& rhs);
	virtual ~CPlayer_Weapon() = default;

public:
	CPlayer::WEAPON		Get_Weapon() { return m_eWeapon; }
	_uint				Get_Key() { return m_iKey; }

	void				Set_isUsing(_bool isUsing) { m_isUsing = isUsing; }
	void				Set_isAttackFrame(_bool isAttack) { m_isAttackFrame = isAttack; }
	void				Set_Key(_uint iKey) { m_iKey = iKey; }
	
public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

protected:
	CPlayer::WEAPON		m_eWeapon = {};
	_bool				m_isUsing = { false };
	_bool				m_isAttackFrame = { false };
	_uint				m_iKey = { 0 };

protected:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CBone*				m_pSocketBone = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };

protected:
	virtual HRESULT Add_Components() = 0;
	HRESULT Bind_ShaderResources();

public:
	virtual void Free() override;
};

END


//class CPlayer_Weapon final : public CPartObject
//{
//public:
//	typedef struct Player_Weapon_Desc : public CPartObject::PARTOBJECT_DESC
//	{
//		CBone*				pSocketBone = { nullptr };
//		wstring				strModelComTag;
//		CPlayer::WEAPON		eWeapon;
//	}PLAYER_WEAPON_DESC;
//
//private:
//	CPlayer_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	CPlayer_Weapon(const CPlayer_Weapon& rhs);
//	virtual ~CPlayer_Weapon() = default;
//
//public:
//	CPlayer::WEAPON	Get_Weapon() { return m_eWeapon; }
//	void			Set_isRender(_bool isRender) { m_isRender = isRender; }
//
//public:
//	virtual HRESULT Initialize_Prototype() override;
//	virtual HRESULT Initialize(void* pArg) override;
//	virtual HRESULT	Tick(_float fTimeDelta) override;
//	virtual void	Late_Tick(_float fTimeDelta) override;
//	virtual HRESULT Render() override;
//
//private:
//	wstring				m_strModelComTag = {};
//	CPlayer::WEAPON		m_eWeapon = {};
//	_bool				m_isRender = { false };
//
//
//private:
//	CModel*			m_pModelCom = { nullptr };
//	CShader*		m_pShaderCom = { nullptr };
//	CBone*			m_pSocketBone = { nullptr };
//	CCollider*		m_pColliderCom = { nullptr };
//
//private:
//	HRESULT Add_Components();
//	HRESULT Bind_ShaderResources();
//
//public:
//	static CPlayer_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	virtual CGameObject* Clone(void* pArg) override;
//	virtual void Free() override;
//
//	// CPartObject을(를) 통해 상속됨
//	void Collision_Event(Engine::CGameObject* pGameObject) override;
//};
//
//END

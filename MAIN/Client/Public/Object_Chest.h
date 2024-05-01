#pragma once

#include "Client_Defines.h"
#include "InteractiveObject.h"
#include "Item.h"

BEGIN(Client)

class CObject_Chest final : public CInteractiveObject
{
public:
	enum ANIMATION {
		ANIM_OPEN, ANIM_AFTER, ANIM_END
	};
	enum STATE {
		STATE_OPEN, STATE_AFTER, STATE_END
	};

	/*enum ITEM {
		ITEM_STICK, ITEM_SWORD, ITEM_SHIELD, ITEM_WAND, ITEM_CUBIC, ITEM_END
	};*/

public:
	typedef struct Chest_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		// 1. Position
		// 2. Item

		//_float4x4			TransformMatrix;
		_vector				vPosition;
		CItem::ITEM_TYPE	eType;
		CItem::ITEM			eItem;
		_bool				isRotation;
		_float				fAngle;
	}CHEST_DESC;

private:
	CObject_Chest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CObject_Chest(const CObject_Chest& rhs);
	virtual ~CObject_Chest() = default;

public:
	_bool			Get_isClose() { return m_isClose; }

	class CItem*	Set_Open() { m_isClose = false; return m_pItem; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool			m_isFirstFrame = { true };
	_bool			m_isClose = { true };
	_bool			m_isFinished = { false };
	_bool			m_isBlend = { false };
	_bool			m_isRotation = { false };

	_float			m_fAngle = { 0.f };

	ANIMATION		m_eAnimationIndex = { ANIM_END };
	ANIMATION		m_eBlendAnimIndex = { ANIM_END };
	STATE			m_eState = { STATE_END };

private:
	class CItem*	m_pItem = { nullptr };
	CCollider*		m_pRigidColliderCom = { nullptr };

private:
	virtual HRESULT	Add_Components();
	virtual HRESULT	Bind_ShaderResources();

public:
	static CObject_Chest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CGameObject을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END
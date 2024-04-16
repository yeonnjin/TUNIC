#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CAnimator;
class CCollider;
class CTransform;
END

/* 귀여운 여우 친구 */

BEGIN(Client)

class CObject_Chest final : public CGameObject
{
public:
	enum ANIMATION {
		ANIM_OPEN, ANIM_AFTER, ANIM_END
	};
	enum STATE {
		STATE_OPEN, STATE_AFTER, STATE_END
	};

	enum ITEM {
		ITEM_STICK, ITEM_SWORD, ITEM_SHIELD, ITEM_WAND, ITEM_CUBIC, ITEM_END
	};

public:
	typedef struct Chest_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		// 1. Position
		// 2. Item

		_float4x4		TransformMatrix;
		
	}CHEST_DESC;

private:
	CObject_Chest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CObject_Chest(const CObject_Chest& rhs);
	virtual ~CObject_Chest() = default;

public:
	void			Change_State(STATE eState);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool			m_isBlend = { false };

	ANIMATION		m_eAnimationIndex = { ANIM_END };
	ANIMATION		m_eBlendAnimIndex = { ANIM_END };
	STATE			m_eState = { STATE_END };

private:
	CAnimator*		m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	CTransform*		m_pLookOnTransform = { nullptr };

private:
	void			Update_State();

private:
	HRESULT			Add_Components();
	HRESULT			Add_States();
	HRESULT			Bind_ShaderResources();
	void			Set_Animation();

public:
	static CObject_Chest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CGameObject을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END
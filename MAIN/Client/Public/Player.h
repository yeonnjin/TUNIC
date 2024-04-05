#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CAnimator;
class CCollider;
class CTransform;
class CPartObject;
class CNavigation;
END

/* 귀여운 여우 친구 */

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum ANIMATION {
		ANIM_STONESWITCH, ANIM_BLINK, ANIM_CLIMB_OFF, ANIM_CLIMB_ON, ANIM_CLIMB,
		ANIM_COIN_FLIP, ANIM_COIN_TOSS, ANIM_DANCE, ANIM_DOOR_APPROACH, ANIM_FOXGOD_APPROACH,
		ANIM_JUMPINTOBED, ANIM_RESURRECTION, ANIM_SLEEPING, ANIM_TAKE_SWORD, ANIM_TAKE_RELIC,
		ANIM_WAKEUP, ANIM_DAGGER, ANIM_DIE, ANIM_DODGE_GARBAGE, ANIM_DODGE,
		ANIM_EAT, ANIM_FALLING, ANIM_GETUP, ANIM_HURT, ANIM_HYPERDASH,
		ANIM_IDLE, ANIM_KNEEL, ANIM_OFFERING, ANIM_OPEN_CHEST, ANIM_PARRY,
		ANIM_PIGGY_SMASH_FAST, ANIM_PIGGY_SMASH, ANIM_SHIELD, ANIM_READY_ARROW, ANIM_SHOOT_ARROW,
		ANIM_SHOTGUN, ANIM_SPEAR_FIRE, ANIM_SPRINT, ANIM_STAGGER, ANIM_SWIG_FLASK,
		ANIM_SWIG_TAKE, ANIM_SWIG, ANIM_SWING_STICK1, ANIM_SWING_STICK2, ANIM_SWING_SWORD1,
		ANIM_SWING_SWORD2, ANIM_SWORD_BOUNCE, ANIM_SWING_SWORD3, ANIM_THUNDER_SUMMON, ANIM_TOSS_ITEM_WINDUP,
		ANIM_TOSS_ITEM, ANIM_USE_KEY, ANIM_USE_WANDBOW, ANIM_WALK_BACKWARD, ANIM_WALK_FORWARD_WATER,
		ANIM_WALK_FORWARD, ANIM_WALK_RIGHT, ANIM_WALK_LEFT, ANIM_WAND_RETRACT, ANIM_WAND_THROW,
		ANIM_WAVE, ANIM_END
	};

	enum STATE { 
		STATE_IDLE, STATE_SLEEP, STATE_MOVE, STATE_ATTACK_STICK, STATE_ATTACK_SWORD, 
		STATE_ATTACK_SHOTGUN, STATE_DAMAGE, STATE_DODGE, STATE_DEFENSE, STATE_END 
	};

	enum WEAPON { 
		WEAPON_STICK, WEAPON_SWORD, WEAPON_SHOTGUN, WEAPON_SHIELD, WEAPON_END 
	};

	enum DIR { 
		DIR_FRONT = 0, DIR_FL, DIR_LEFT, DIR_BL, DIR_BACK, 
		DIR_BR, DIR_RIGHT, DIR_FR, DIR_END 
	};

	enum STATUS { STATUS_HURT, STATUS_STAGGER, STATUS_PARRY, STATUS_END };
	enum DODGE { DODGE_ROLL, DODGE_FAST, DODGE_DASH, DODGE_END };
	enum LOCKON { LOCK_ON_FIND, LOCK_ON_NONE, LOCK_OFF, LOCK_END };

	/*enum STATE {
		STATE_IDLE = 0x01,
		STATE_RUN = 0x02,
		STATE_ATTCK = 0x04,
	};*/

public: // GAMEOBJECT_DESC 로 올리면 카메라에서 터짐;;
	typedef struct Player_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4x4		TransformMatrix;
		wstring			strModelComTag;
	}PLAYER_DESC;

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	// Set
	void			Set_Level(_uint iLevel) { m_iLevel = iLevel; }
	void			Set_Blending(_bool isBlend, ANIMATION eBlendAnimIndex) { m_isBlend = isBlend; m_eBlendAnimIndex = eBlendAnimIndex; }
	void			Set_AnimationIndex(ANIMATION eAnimIndex) { m_eAnimationIndex = eAnimIndex; }
	void			Set_Weapon_Render(const wstring& strWeaponTag, _bool isRender);
	void			Set_Parrying(_bool isParrying) { m_isParrying = isParrying; }
	void			Set_LockOn(LOCKON eLockOn) { m_eLockOn = eLockOn; }

	// Get
	//STATE			Get_State() { return m_eState; }
	_bool			isMove() { return m_eState == STATE_IDLE ? false : true; }
	_bool			isAttack() { return m_eState == STATE_ATTACK_STICK || m_eState == STATE_ATTACK_SWORD || m_eState == STATE_ATTACK_SHOTGUN ? true : false; }
	_bool			isParrying() { return m_isParrying; }

	DIR				Get_Dir() { return m_eDir; }
	_vector			Get_Look() { return XMVector3Normalize(m_vLook); }
	STATE			Get_State() { return m_eState; }
	STATUS			Get_Status() { return m_eStatus; }
	DODGE			Get_Dodge() { return m_eDodge; }
	LOCKON			Get_LockOn() { return m_eLockOn; }
	_bool			Get_isFinished(ANIMATION eAnimIndex) { return m_pModelCom->isFinished(eAnimIndex); }
	CTransform*		Get_LockOn_Transform() { return m_pLookOnTransform; }

	// State
	void			Change_State(STATE eState);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	map<const wstring, CPartObject*>	m_PartObjects;
	_uint								m_iLevel = { LEVEL_END };
	
	_bool								m_isBlend = { false };
	_bool								m_isParrying = { false };

	_vector								m_vPrePosition = {};

private:
	ANIMATION							m_eAnimationIndex = { ANIM_END };
	ANIMATION							m_eBlendAnimIndex = { ANIM_END };

	STATE								m_eState = { STATE_END };
	DIR									m_eDir = { DIR_END };
	_vector								m_vLook = {};
	STATUS								m_eStatus = { STATUS_HURT };	// 바이트?
	DODGE								m_eDodge = { DODGE_END };
	WEAPON								m_eWeapon = { WEAPON_END };
	LOCKON								m_eLockOn = { LOCK_END };

private:
	wstring								m_strModelComTag = {};
	CAnimator*							m_pModelCom = { nullptr };
	CShader*							m_pShaderCom = { nullptr };
	CCollider*							m_pColliderCom = { nullptr };
	CNavigation*						m_pNavigationCom = { nullptr };
	CTransform*							m_pLookOnTransform = { nullptr };

private:
	void			Update_State();
	void			Update_Camera();
	
private:
	HRESULT			Add_Components();
	HRESULT			Add_PartObjects();
	HRESULT			Add_States();
	HRESULT			Bind_ShaderResources();
	void			Set_Animation();
	void			Set_Dir();
	void			Set_Weapon();
	CTransform*		Set_LockOn_Target();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CGameObject을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END
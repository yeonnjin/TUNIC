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
		STATE_ATTACK_SHOTGUN, STATE_ATTACK_WAND, STATE_DAMAGE, STATE_DODGE, STATE_DEFENSE,
		STATE_OPEN, STATE_PUZZLE, STATE_CLIMB, STATE_TOP,
		STATE_END
	};

	enum WEAPON { 
		WEAPON_STICK, WEAPON_SWORD, WEAPON_WAND, WEAPON_SHIELD, WEAPON_DASH, WEAPON_END 
	};

	enum DIR { 
		DIR_FRONT = 0, DIR_FL, DIR_LEFT, DIR_BL, DIR_BACK, 
		DIR_BR, DIR_RIGHT, DIR_FR, DIR_END 
	};

	enum STATUS { STATUS_HURT, STATUS_STAGGER, STATUS_PARRY, STATUS_END };
	enum DODGE { DODGE_ROLL, DODGE_FAST, DODGE_DASH, DODGE_END };
	enum LOCKON { LOCK_ON_FIND, LOCK_ON_NONE, LOCK_OFF, LOCK_END };
	enum CLIMB { CLIMB_ENTER, CLIMB_UPPER, CLIMB_LOWER, CLIMB_END };

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
	HRESULT			Set_Navigation(_uint iLevel);
	void			Set_Blending(_bool isBlend, ANIMATION eBlendAnimIndex) { m_isBlend = isBlend; m_eBlendAnimIndex = eBlendAnimIndex; }
	void			Set_AnimationIndex(ANIMATION eAnimIndex) { m_eAnimationIndex = eAnimIndex; }
	void			Set_AnimationData_Initialize(ANIMATION eAnimIndex) { m_pModelCom->Set_AnimationData_Initialize(eAnimIndex); }
	void			Set_Play_Animation_Reverse(ANIMATION eAnimIndex) { m_pModelCom->Set_AnimationData_Reverse(eAnimIndex); }
	void			Set_Weapon_Render(const wstring& strWeaponTag, _bool isRender);
	void			Set_Parrying(_bool isParrying) { m_isParrying = isParrying; }
	void			Set_LockOn(LOCKON eLockOn) { m_eLockOn = eLockOn; }
	void			Set_UtileItem(WEAPON eWeapon);

	void			Set_StopAnimation(_bool isStop) { m_isStop = isStop; }
	void			Set_Climb(CLIMB eClimb) { m_eClimb = eClimb; }
	void			Set_Arrive(_bool isArrive) { m_isArrive = isArrive; }

	void			Set_SP_Minus(_float fSPMinus) { m_fSP -= fSPMinus; m_fAccSPTime = 0.f; }
	void			Set_MP_Minus(_float fMPMinus) { m_fMP -= fMPMinus; }
	void			Set_MP(_float fMPPlus) { m_fMP += fMPPlus; }
	void			Set_ChestOpen() { m_isChestOpen = true; }
	void			Set_LadderUpper(_bool isUpper) { m_isUpper = isUpper; }
	//void			Set_Climb() { m_isClimb; }

	void			Set_Gem();
	void			Set_LockOff();

	// Get
	//STATE			Get_State() { return m_eState; }
	_bool			isMove() { return m_eState == STATE_IDLE ? false : true; }
	_bool			isAttack() { return m_eState == STATE_ATTACK_STICK || m_eState == STATE_ATTACK_SWORD || m_eState == STATE_ATTACK_SHOTGUN ? true : false; }
	_bool			isParrying() { return m_isParrying; }
	//_bool			isPuzzle() { return m_isPuzzle; }
	//_bool			isAttacking() { return m_isAttcking; }
	_bool			isUpper() { return m_isUpper; }
	_bool			isArrive() { return m_isArrive; }
	_bool			isEndLadder() { return m_isEndLadder; }

	_float			Get_SP() { return m_fSP; }
	_float			Get_MP() { return m_fMP; }
	_uint			Get_MaxHP() { return m_iMaxHP; }
	_float			Get_MaxMP() { return m_fMaxMP; }

	DIR				Get_Dir() { return m_eDir; }
	_vector			Get_Look() { return XMVector3Normalize(m_vLook); }
	STATE			Get_State() { return m_eState; }
	STATUS			Get_Status() { return m_eStatus; }
	DODGE			Get_Dodge() { return m_eDodge; }
	LOCKON			Get_LockOn() { return m_eLockOn; }
	CLIMB			Get_Climb() { return m_eClimb; }

	_bool			Get_isFinished(ANIMATION eAnimIndex) { return m_pModelCom->isFinished(eAnimIndex); }
	CTransform*		Get_LockOn_Transform() { return m_pLookOnTransform; }
	_uint			Get_Current_Frame(_uint eAnimationIndex) { return m_pModelCom->Get_Current_Frame(eAnimationIndex); }
	_uint			Get_Ladder_Index() { return m_iLadderIndex; }

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
	//_bool								m_isAttackFrame = { false };
	_bool								m_isCanChange = { true };
	//_bool								m_isChanged = { false };
	_bool								m_isUsingInventory = { false };
	_bool								m_isInteractive = { false };
	_bool								m_isChestOpen = { false };
	_bool								m_isUsingShop = { false };
	_bool								m_isObtain = { false };
	//_bool								m_isAttcking = { false };
	//_bool								m_isPuzzle = { false };
	_bool								m_isStop = { false };

	_bool								m_isUpper = { false };
	_bool								m_isArrive = { false };
	_bool								m_isEndLadder = { false };

	_float								m_fAccChageTime = { 0.f };
	_float								m_fChangeTime = { 0.21f };

	_vector								m_vPrePosition = {};
	_vector								m_vLook = {};

private:
	_uint								m_iMaxHP = { 7 };
	_float								m_fSP = { 4.f };
	_float								m_fMaxSP = { 4.f };
	_float								m_fMP = { 4.f };
	_float								m_fMaxMP = { 4.f };

	_float								m_fAccSPTime = { 0.f };
	_float								m_fSPTime = { 2.f };

	_uint								m_iLadderIndex = { 0 };

private:
	ANIMATION							m_eAnimationIndex = { ANIM_END };
	ANIMATION							m_eBlendAnimIndex = { ANIM_END };
	STATE								m_eState = { STATE_END };
	DIR									m_eDir = { DIR_END };
	STATUS								m_eStatus = { STATUS_HURT };	// 바이트?
	DODGE								m_eDodge = { DODGE_ROLL };
	WEAPON								m_eWeapon = { WEAPON_END };
	LOCKON								m_eLockOn = { LOCK_END };
	CLIMB								m_eClimb = { CLIMB_END };

private:
	wstring								m_strModelComTag = {};
	CAnimator*							m_pModelCom = { nullptr };
	CShader*							m_pShaderCom = { nullptr };
	CCollider*							m_pColliderCom = { nullptr };
	CCollider*							m_pRigidColliderCom = { nullptr };
	CNavigation*						m_pNavigationCom = { nullptr };
	CTransform*							m_pLookOnTransform = { nullptr };

	class CUI_Stat*						m_pUI_Stat = { nullptr };
	class CUI_LockOn*					m_pUI_LockOn = { nullptr };
	class CInventory*					m_pInventory = { nullptr };

private:
	void								Update_State();
	void								Update_Camera();
	
private:
	HRESULT								Add_Components();
	HRESULT								Add_PartObjects();
	HRESULT								Add_States();
	HRESULT								Bind_ShaderResources();
	void								Set_Animation();
	void								Set_Dir();
	void								Set_Weapon(_uint iKey);
	CTransform*							Set_LockOn_Target();
	void								Compute_Stat_Gauge(_float fTimeDelta);
	void								Compute_Height();
	class CPlayer_Weapon*				Find_Weapon(WEAPON eWeapon);

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CGameObject을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
	void Damage_Event() override;

	//void Rigid_Event(Engine::CGameObject* pGameObject) override;
};

END
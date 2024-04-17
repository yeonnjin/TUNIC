#include "stdafx.h"
#include "Player.h"

#include "Weapon_Stick.h"
#include "Weapon_Sword.h"
#include "Weapon_Shield.h"
#include "Weapon_Wand.h"

// State
#include "Player_State_Idle.h"
#include "Player_State_Sleep.h"
#include "Player_State_Move.h"
#include "Player_State_Attack_Stick.h"
#include "Player_State_Attack_Sword.h"
#include "Player_State_Attack_Shotgun.h"
#include "Player_State_Attack_Wand.h"
#include "Player_State_Damage.h"
#include "Player_State_Dodge.h"
#include "Player_State_Defense.h"

// Camera
#include "Camera_LockOn.h"

// UI
#include "UI_Stat.h"
#include "Inventory.h"

#define	WEAPONBONEIDX 45
#define SHIELDBONE 24
#define STICKBONE 28
#define SWORDBONE 28
#define WANDBONE 28
#define SHOTGUNBONE 45
// stick - 29 / sword - 45 / shield - 24 / Shotgun - 45?

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 8.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		PLAYER_DESC* pDesc = (PLAYER_DESC*)pArg;
		m_strModelComTag = pDesc->strModelComTag;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_States()))
		return E_FAIL;

	m_eType = OBJ_PLAYER;
	m_fDamageCoolTime = 1.f;

	_float4 vPosition = _float4(0.f, 0.2f, 0.f, 1.f);  /*_float4(3.f, 0.2f, 50.f, 1.f);*/ /*_float4(-75.f, 3.f, 68.f, 1.f);*/ /*_float4(-66.f, 2.f, 62.f, 1.f); */
	m_vPrePosition = XMLoadFloat4(&vPosition);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pModelCom->Set_Animation_Index(ANIM_IDLE);
	m_pModelCom->Set_Animation_Transform(m_pTransformCom);
	Set_Animation();

	m_iHP = 7;
	m_pUI_Stat = dynamic_cast<CUI_Stat*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_UI_Stat")));
	if (nullptr == m_pUI_Stat)
		return E_FAIL;
	Safe_AddRef(m_pUI_Stat);

	m_pInventory = CInventory::Create();
	if (nullptr == m_pInventory)
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Tick(_float fTimeDelta)
{
	if (E_FAIL == __super::Tick(fTimeDelta))
		return E_FAIL;

	Set_Dir();
	Set_Weapon();
	
	// State_Machine
	m_pModelCom->Update_State(fTimeDelta);
	Update_State();
	Update_Camera();

	m_fAccChageTime += fTimeDelta;
	if (false == m_isCanChange && m_fAccChageTime > m_fChangeTime)
	{
		m_isCanChange = true;
	}

	// Blending
	if (true == m_isBlend)
	{
		if (S_OK == m_pModelCom->Blending_Animation(m_eBlendAnimIndex, fTimeDelta))
		{
			m_isBlend = false;
			m_pModelCom->Set_Animation_Index(m_eBlendAnimIndex);
			m_eAnimationIndex = m_eBlendAnimIndex;
		}
	}	
	else
		m_pModelCom->Play_Animation(fTimeDelta);

	// Collider
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	// Navigation
	if (false == m_pNavigationCom->isMove(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION)))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPrePosition);
	}
	m_vPrePosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	// Stat
	Compute_Stat_Gauge(fTimeDelta);

	// PartObject
	for (auto& PartObject : m_PartObjects)
		PartObject.second->Tick(fTimeDelta);

	// Inventory
	if (true == m_pGameInstance->Get_DIKeyState(DIK_U, KEY_DOWN))
	{
		m_isUsingInventory = !m_isUsingInventory;
		m_pInventory->Set_Using(m_isUsingInventory);
	}
	m_pInventory->Tick(fTimeDelta);

	m_pGameInstance->Add_Group(CCollision_Manager::GROUP_PLAYER, this);

	return S_OK;
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	for (auto& PartObject : m_PartObjects)
		PartObject.second->Late_Tick(fTimeDelta);

	/*if (true == m_pColliderCom->Check_Collision((CCollider*)m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Com_Collider"))))
	{
		m_pModelCom->Set_SlowMotion(ANIM_SWING_STICK1, 16, 26, 0.2f);
	}
	else
		m_pModelCom->Set_SlowMotion(ANIM_SWING_STICK1, 16, 26, 0.f);*/

	Compute_Damage_CoolTime(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	//for (auto& pColliderCom : m_pColliderCom)
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CPlayer::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CPlayer::Update_State()
{
	switch (m_eState)
	{
	case STATE_IDLE:
		// IDLE -> MOVE
		if (m_pGameInstance->Get_DIKeyState(DIK_W, KEY_PRESS) ||
			m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS) ||
			m_pGameInstance->Get_DIKeyState(DIK_A, KEY_PRESS) ||
			m_pGameInstance->Get_DIKeyState(DIK_D, KEY_PRESS))
		{
			Change_State(STATE_MOVE);			
		}

		if (m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON, KEY_DOWN))
		{
			switch (m_eWeapon)
			{
			case WEAPON_STICK:
				Change_State(STATE_ATTACK_STICK);
				break;
			case WEAPON_SWORD:
				Change_State(STATE_ATTACK_SWORD);
				break;
			case WEAPON_SHOTGUN:
				Change_State(STATE_ATTACK_SHOTGUN);
				break;
			case WEAPON_WAND:
				if (m_fMP > 0.8f)
					Change_State(STATE_ATTACK_WAND);
			case WEAPON_END:
				break;
			default:
				break;
			}
		}
		 
		if (m_pGameInstance->Get_DIKeyState(DIK_SPACE, KEY_DOWN))
		{
			m_eDodge = DODGE_ROLL;
			Change_State(STATE_DODGE);
		}

		if (m_pGameInstance->Get_DIKeyState(DIK_O, KEY_DOWN))
			Change_State(STATE_SLEEP);

		if (m_pGameInstance->Get_DIKeyState(DIK_K, KEY_DOWN))
			Change_State(STATE_DAMAGE);

		if (m_pGameInstance->Get_DIMouseState(DIMKS_RBUTTON, KEY_DOWN))
			Change_State(STATE_DEFENSE);

		break;

	case STATE_MOVE:
		if (!m_pGameInstance->Get_DIKeyState(DIK_W, KEY_PRESS) &&
			!m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS) &&
			!m_pGameInstance->Get_DIKeyState(DIK_A, KEY_PRESS) &&
			!m_pGameInstance->Get_DIKeyState(DIK_D, KEY_PRESS))
		{
			//m_eDir = DIR_FORWARD;
			Change_State(STATE_IDLE);
		}

		if (m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON, KEY_DOWN))
		{
			switch (m_eWeapon)
			{
			case WEAPON_STICK:
				Change_State(STATE_ATTACK_STICK);
				break;
			case WEAPON_SWORD:
				Change_State(STATE_ATTACK_SWORD);
				break;
			case WEAPON_SHOTGUN:
				Change_State(STATE_ATTACK_SHOTGUN);
				break;
			case WEAPON_WAND:
				if(m_fMP >= 0.8f)
					Change_State(STATE_ATTACK_WAND);	
				break;
			case WEAPON_END:
				break;
			default:
				break;
			}
		}

		if (m_pGameInstance->Get_DIMouseState(DIMKS_RBUTTON, KEY_DOWN))
			Change_State(STATE_DEFENSE);

		if (m_pGameInstance->Get_DIKeyState(DIK_SPACE, KEY_DOWN))
		{
			m_eDodge = DODGE_ROLL;
			Change_State(STATE_DODGE);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_LCONTROL, KEY_DOWN))
		{
			m_eDodge = DODGE_FAST;
			Change_State(STATE_DODGE);
		}
		/*if (m_pGameInstance->Get_DIKeyState(DIK_C, KEY_DOWN))
		{
			m_eDodge = DODGE_DASH;
			Change_State(STATE_DODGE);
		}*/

		break;
	case STATE_END:
		break;
	default:
		break;
	}
}

void CPlayer::Update_Camera()
{
	if ((LOCK_OFF == m_eLockOn || LOCK_END == m_eLockOn) && true == m_pGameInstance->Get_DIKeyState(DIK_LSHIFT, KEY_DOWN))
	{
		CTransform* pLockOnTransform = Set_LockOn_Target();
		if (nullptr != pLockOnTransform)
		{
			CCamera_LockOn::CAMERA_LOCKON_DESC tDesc{};
			tDesc.vTargetTransform = pLockOnTransform;
			m_pGameInstance->Change_Camera(TEXT("Camera_LockOn"), &tDesc);
			m_eLockOn = LOCK_ON_FIND;
		}
		else
		{
			m_pGameInstance->Change_Camera(TEXT("Camera_LockOn"));
			m_eLockOn = LOCK_ON_NONE;
		}
	}

	if((LOCK_ON_FIND == m_eLockOn || LOCK_ON_NONE == m_eLockOn )&& true == m_pGameInstance->Get_DIKeyState(DIK_LSHIFT, KEY_UP))
	{
		m_pGameInstance->Set_Exit(TEXT("Camera_LockOn"), true);
		m_eLockOn = LOCK_OFF;

		IF_PLAYER_ISMOVE
			Change_State(CPlayer::STATE_MOVE);
	}
}

void CPlayer::Change_State(STATE eState)
{
	if(true == m_isCanChange)
	{
		m_pModelCom->Change_State(eState);
		m_eState = eState;

		m_isCanChange = false;
		m_fAccChageTime = 0.f;
	}
}	

void CPlayer::Set_Weapon_Render(const wstring& strWeaponTag, _bool isRender)
{
	CPartObject* pWeapon = m_PartObjects.find(strWeaponTag)->second;
	if (nullptr == pWeapon)
		return;

	dynamic_cast<CPlayer_Weapon*>(pWeapon)->Set_isUsing(isRender);
}

//_bool CPlayer::isAttack()
//{
//	if (m_eAnimationIndex == ANIM_SWING_SWORD1)
//	{
//		_uint iIndex = m_pModelCom->Get_Current_Frame(ANIM_SWING_SWORD1);
//		if (15 <= iIndex && 40 >= iIndex)
//			return true;
//	}
//	else if(m_eAnimationIndex == ANIM_SWING_SWORD2)
//	{
//		_uint iIndex = m_pModelCom->Get_Current_Frame(ANIM_SWING_SWORD2);
//		if (5 <= iIndex && 25 >= iIndex)
//			return true;
//	}
//	else if (m_eAnimationIndex == ANIM_SWING_SWORD3)
//	{
//		_uint iIndex = m_pModelCom->Get_Current_Frame(ANIM_SWING_SWORD3);
//		if (15 <= iIndex && 55 >= iIndex)
//			return true;
//	}
//
//
//	return false;
//}

HRESULT CPlayer::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_strModelComTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For. Com_Collider */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC ColliderDesc{};
	
	ColliderDesc.fRadius = 0.8f;
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius + 0.6f , 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NavigationDesc{};
	NavigationDesc.iCurrentIndex = 0;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CPlayer::Add_PartObjects()
{
	/* For. Part_Player_Weapon_Stick */
	CPartObject* pWeaponObject = { nullptr };
	CPlayer_Weapon::PLAYER_WEAPON_DESC tDesc{};

	tDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	tDesc.pSocketBone = m_pModelCom->Get_Bone_Ptr(STICKBONE);
	tDesc.eWeapon = WEAPON_STICK;

	pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Part_Player_Weapon_Stick"), &tDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;

	m_PartObjects.emplace(TEXT("Part_Player_Weapon_Stick"), pWeaponObject);

	/* For. Part_Player_Weapon_Sword */
	pWeaponObject = { nullptr };
	tDesc = {};

	tDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	tDesc.pSocketBone = m_pModelCom->Get_Bone_Ptr(SWORDBONE);
	tDesc.eWeapon = WEAPON_SWORD;

	pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Part_Player_Weapon_Sword"), &tDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;

	m_PartObjects.emplace(TEXT("Part_Player_Weapon_Sword"), pWeaponObject);

	/* For. Part_Player_Weapon_Wand */
	pWeaponObject = { nullptr };
	tDesc = {};

	tDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	tDesc.pSocketBone = m_pModelCom->Get_Bone_Ptr(SWORDBONE);
	tDesc.eWeapon = WEAPON_WAND;

	pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Part_Player_Weapon_Wand"), &tDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;

	m_PartObjects.emplace(TEXT("Part_Player_Weapon_Wand"), pWeaponObject);

	///* For. Part_Player_Weapon_Shotgun */
	//pWeaponObject = { nullptr };
	//tDesc = {};

	//tDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	//tDesc.pSocketBone = m_pModelCom->Get_Bone_Ptr(SHOTGUNBONE);
	//_char szModelTag2[MAX_PATH] = "Prototype_Component_Model_Weapon_Shotgun";
	//wstring wstr2(&szModelTag2[0], &szModelTag2[MAX_PATH]);
	//tDesc.strModelComTag = wstr2;
	//tDesc.eWeapon = WEAPON_SHOTGUN;

	//pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Part_Player_Weapon"), &tDesc));
	//if (nullptr == pWeaponObject)
	//	return E_FAIL;

	//m_PartObjects.emplace(TEXT("Part_Player_Weapon_Shotgun"), pWeaponObject);

	///* For. Part_Player_Weapon_Wandbow */
	//pWeaponObject = { nullptr };
	//tDesc = {};

	//tDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	//tDesc.pSocketBone = m_pModelCom->Get_Bone_Ptr(WANDBONE);
	//_char szModelTag3[MAX_PATH] = "Prototype_Component_Model_Weapon_Wandbow";
	//wstring wstr3(&szModelTag3[0], &szModelTag3[MAX_PATH]);
	//tDesc.strModelComTag = wstr3;
	//tDesc.eWeapon = WEAPON_WAND;

	//pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Part_Player_Weapon"), &tDesc));
	//if (nullptr == pWeaponObject)
	//	return E_FAIL;

	//m_PartObjects.emplace(TEXT("Part_Player_Weapon_Wandbow"), pWeaponObject);

	/* For. Part_Player_Weapon_Shield */
	pWeaponObject = { nullptr };
	tDesc = {};

	tDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	tDesc.pSocketBone = m_pModelCom->Get_Bone_Ptr(SHIELDBONE);
	tDesc.eWeapon = WEAPON_SHIELD;

	pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Part_Player_Weapon_Shield"), &tDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;

	m_PartObjects.emplace(TEXT("Part_Player_Weapon_Shield"), pWeaponObject);

	return S_OK;
}

HRESULT CPlayer::Add_States()
{
	m_pModelCom->Add_State(STATE_IDLE, CPlayer_State_Idle::Create(this));
	m_pModelCom->Add_State(STATE_SLEEP, CPlayer_State_Sleep::Create(this));
	m_pModelCom->Add_State(STATE_MOVE, CPlayer_State_Move::Create(this));
	//m_pModelCom->Add_State(STATE_LOCKON, CPlayer_State_LockOn::Create(this));
	m_pModelCom->Add_State(STATE_ATTACK_STICK, CPlayer_State_Attack_Stick::Create(this, dynamic_cast<CPlayer_Weapon*>(m_PartObjects.find(TEXT("Part_Player_Weapon_Stick"))->second)));
	m_pModelCom->Add_State(STATE_ATTACK_SWORD, CPlayer_State_Attack_Sword::Create(this, dynamic_cast<CPlayer_Weapon*>(m_PartObjects.find(TEXT("Part_Player_Weapon_Sword"))->second)));
	m_pModelCom->Add_State(STATE_ATTACK_WAND, CPlayer_State_Attack_Wand::Create(this, dynamic_cast<CPlayer_Weapon*>(m_PartObjects.find(TEXT("Part_Player_Weapon_Wand"))->second)));
	//m_pModelCom->Add_State(STATE_ATTACK_SHOTGUN, CPlayer_State_Attack_Shotgun::Create(this, dynamic_cast<CPlayer_Weapon*>(m_PartObjects.find(TEXT("Part_Player_Weapon_Shotgun"))->second)));
	m_pModelCom->Add_State(STATE_DAMAGE, CPlayer_State_Damage::Create(this));
	m_pModelCom->Add_State(STATE_DODGE, CPlayer_State_Dodge::Create(this));
	m_pModelCom->Add_State(STATE_DEFENSE, CPlayer_State_Defense::Create(this));
	m_pModelCom->Change_State(STATE_IDLE);
	m_eState = STATE_IDLE;

	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = m_pGameInstance->Get_Camera_Far();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Set_Animation()
{
	// LOOP
	m_pModelCom->Set_Animation_isLoop(ANIM_IDLE, true);
	m_pModelCom->Set_Animation_isLoop(ANIM_SLEEPING, true);
	m_pModelCom->Set_Animation_isLoop(ANIM_WALK_FORWARD, true);
	m_pModelCom->Set_Animation_isLoop(ANIM_WALK_BACKWARD, true);
	m_pModelCom->Set_Animation_isLoop(ANIM_WALK_LEFT, true);
	m_pModelCom->Set_Animation_isLoop(ANIM_WALK_RIGHT, true);
	m_pModelCom->Set_Animation_isLoop(ANIM_SHIELD, true);

	// ROOT
	m_pModelCom->Set_Animation_isRoot(ANIM_SWING_STICK1, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_SWING_STICK2, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_SWING_SWORD1, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_SWING_SWORD2, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_SWING_SWORD3, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_SHOTGUN, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_USE_WANDBOW, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_HURT, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_STAGGER, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_DODGE, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_DODGE_GARBAGE, true);

	// BLEND TIME
	m_pModelCom->Set_Blend_Time(ANIM_SWING_STICK1, 0.2f);
	m_pModelCom->Set_Blend_Time(ANIM_SWING_STICK2, 0.1f);
	m_pModelCom->Set_Blend_Time(ANIM_SWING_SWORD1, 0.1f);
	m_pModelCom->Set_Blend_Time(ANIM_SWING_SWORD2, 0.1f);
	m_pModelCom->Set_Blend_Time(ANIM_SWING_SWORD3, 0.1f);
	m_pModelCom->Set_Blend_Time(ANIM_DODGE, 0.1f);
	m_pModelCom->Set_Blend_Time(ANIM_STAGGER, 0.4f);

	// TICK WEIGHT
	m_pModelCom->Set_Frame_Tick(ANIM_DODGE, 0, 15, 1.4f);
	m_pModelCom->Set_Frame_Tick(ANIM_DODGE, 35, 51, 60.f);
	m_pModelCom->Set_Frame_Tick(ANIM_DODGE_GARBAGE, 31, 37, 50.f);
	m_pModelCom->Set_Frame_Tick(ANIM_SWING_STICK2, 36, 39, 80.f);
	m_pModelCom->Set_Frame_Tick(ANIM_SWING_SWORD3, 60, 67, 50.f);

	// SLOWMOTION
	//m_pModelCom->Set_Frame_Tick(ANIM_SWING_SWORD1, 16, 26, 0.2f);
	//m_pModelCom->Set_SlowMotion(ANIM_DODGE_GARBAGE, 3, 15, 0.2f);
}

void CPlayer::Set_Dir()
{
	// 8방향
	m_eDir = DIR_END;

	if (m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS))
	{
		m_eDir = DIR_FRONT;
		m_vLook = { 0.f, 0.f, 1.f };
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_W, KEY_PRESS))
	{
		m_eDir = DIR_BACK;
		m_vLook = { 0.f, 0.f, -1.f };
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_A, KEY_PRESS))
	{
		m_eDir = DIR_LEFT;
		m_vLook = { 1.f, 0.f, 0.f };
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_D, KEY_PRESS))
	{
		m_eDir = DIR_RIGHT;
		m_vLook = { -1.f, 0.f, 0.f };
	}
	
	if (m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS) && m_pGameInstance->Get_DIKeyState(DIK_A, KEY_PRESS))
	{
		m_eDir = DIR_FL;
		m_vLook = { 1.f, 0.f, 1.f };
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS) && m_pGameInstance->Get_DIKeyState(DIK_D, KEY_PRESS))
	{
		m_eDir = DIR_FR;
		m_vLook = { -1.f, 0.f, 1.f };
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_W, KEY_PRESS) && m_pGameInstance->Get_DIKeyState(DIK_A, KEY_PRESS))
	{
		m_eDir = DIR_BL;
		m_vLook = { 1.f, 0.f, -1.f };
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_W, KEY_PRESS) && m_pGameInstance->Get_DIKeyState(DIK_D, KEY_PRESS))
	{
		m_eDir = DIR_BR;
		m_vLook = { -1.f, 0.f, -1.f };
	}
}

void CPlayer::Set_Weapon()
{
	if (m_pGameInstance->Get_DIKeyState(DIK_Z, KEY_DOWN))
	{
		m_eWeapon = WEAPON_STICK;
		static _bool isStick = false;
		isStick = !isStick;
		if (true == isStick)
			Set_Weapon_Render(TEXT("Part_Player_Weapon_Stick"), true);
		else
			Set_Weapon_Render(TEXT("Part_Player_Weapon_Stick"), false);
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_X, KEY_DOWN))
	{
		m_eWeapon = WEAPON_SWORD;
		static _bool isSword = false;
		isSword = !isSword;
		if (true == isSword)
			Set_Weapon_Render(TEXT("Part_Player_Weapon_Sword"), true);
		else
			Set_Weapon_Render(TEXT("Part_Player_Weapon_Sword"), false);
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_C, KEY_DOWN))
	{
		m_eWeapon = WEAPON_SHOTGUN;
		static _bool isShotgun = false;
		isShotgun = !isShotgun;
		if (true == isShotgun)
			Set_Weapon_Render(TEXT("Part_Player_Weapon_Shotgun"), true);
		else
			Set_Weapon_Render(TEXT("Part_Player_Weapon_Shotgun"), false);
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_V, KEY_DOWN))
	{
		m_eWeapon = WEAPON_WAND;
		static _bool isWandbow = false;
		isWandbow = !isWandbow;
		if (true == isWandbow)
			Set_Weapon_Render(TEXT("Part_Player_Weapon_Wand"), true);
		else
			Set_Weapon_Render(TEXT("Part_Player_Weapon_Wand"), false);
	}
	
	static _bool isShield = false;
	if (m_pGameInstance->Get_DIKeyState(DIK_B, KEY_DOWN))
	{		
		isShield = !isShield;
		if (true == isShield)
			Set_Weapon_Render(TEXT("Part_Player_Weapon_Shield"), true);
		else
			Set_Weapon_Render(TEXT("Part_Player_Weapon_Shield"), false);
	}

	if(WEAPON_STICK == m_eWeapon)
		m_pGameInstance->Add_Group(CCollision_Manager::GROUP_PLAYER_WEAPON, m_PartObjects.find(TEXT("Part_Player_Weapon_Stick"))->second);
	else if(WEAPON_SWORD == m_eWeapon)
		m_pGameInstance->Add_Group(CCollision_Manager::GROUP_PLAYER_WEAPON, m_PartObjects.find(TEXT("Part_Player_Weapon_Sword"))->second);
	else if (WEAPON_SHOTGUN == m_eWeapon)
		m_pGameInstance->Add_Group(CCollision_Manager::GROUP_PLAYER_WEAPON, m_PartObjects.find(TEXT("Part_Player_Weapon_Shotgun"))->second);

	if(true == isShield)
		m_pGameInstance->Add_Group(CCollision_Manager::GROUP_PLAYER_WEAPON, m_PartObjects.find(TEXT("Part_Player_Weapon_Shield"))->second);
}

CTransform* CPlayer::Set_LockOn_Target()
{
	CTransform* pTargetTransform = nullptr;

	// 최대 거리
	_float fDistance = 20.f;
	_vector vPlayerPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	// 플레이어와의 거리
	_uint iNumMonsters = m_pGameInstance->Get_Object_Count(m_iLevel, TEXT("Layer_Monster"));
	for (size_t i = 0; i < iNumMonsters; i++)
	{
		CTransform* pMonsterTransform = (CTransform*)(m_pGameInstance->Get_Component(m_iLevel, TEXT("Layer_Monster"), g_strTransformTag, i));
		_vector vMonsterPosition = pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
		_float fDiff = XMVector3Length(vMonsterPosition - vPlayerPosition).m128_f32[0];

		if (fDistance > fDiff)
		{
			fDistance = fDiff;
			pTargetTransform = pMonsterTransform;
		}
	}

	m_pLookOnTransform = pTargetTransform;

	return pTargetTransform;
}

void CPlayer::Compute_Stat_Gauge(_float fTimeDelta)
{
	m_fAccSPTime += fTimeDelta;

	if (m_fSP < m_fMaxSP && m_fAccSPTime > m_fSPTime)
	{
		m_fSP += 0.1f;
	}
	else if (m_fSP >= m_fMaxSP)
	{
		m_fAccSPTime = 0.f;
		m_fSP = m_fMaxSP;
	}

	if (true == m_pGameInstance->Get_DIKeyState(DIK_9, KEY_DOWN))
	{
		m_fMP += 0.8f;
		if (m_fMP > m_fMaxMP)
			m_fMP = m_fMaxMP;
	}

	if (m_iHP < 0)
		m_iHP = 0;

	m_pUI_Stat->Set_Stat(m_iHP, m_fSP, m_fMP);
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CPlayer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CPlayer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& PartObject : m_PartObjects)
		Safe_Release(PartObject.second);

	m_PartObjects.clear();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);

	Safe_Release(m_pUI_Stat);
	Safe_Release(m_pInventory);
}

void CPlayer::Collision_Event(Engine::CGameObject* pGameObject)
{
}

void CPlayer::Damage_Event()
{
}

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
#include "Player_State_Open.h"
#include "Player_State_Puzzle.h"
#include "Player_State_Climb.h"
#include "Player_State_Top.h"

// Camera
#include "Camera_LockOn.h"

// UI
#include "UI_Stat.h"
#include "UI_Obtain.h"
#include "Inventory.h"

// Interactive
#include "Item.h"
#include "Object_Chest.h"
#include "Object_Ladder.h"

// TEST
//#include "Particle_Red.h"

#define	WEAPONBONEIDX 45
#define SHIELDBONE 24
#define STICKBONE 28
#define SWORDBONE 28
#define WANDBONE 28
#define SHOTGUNBONE 45
#define DASHBONE 79
// stick - 29 / sword - 45 / shield - 24 / Shotgun - 45? /  Dash - 79

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

	GameObjectDesc.fSpeedPerSec = 6.f;
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
	m_eRigid = RIGID_BLOCK;

	m_fDamageCoolTime = 1.f;

	// FOXGOD : _float4(0.f, 0.2f, 0.f, 1.f);
	// BEACH :  _float4(-62.f, 2.f, 62.f, 1.f); _float4(65.f, 2.f, -62.f, 1.f); 
	// LIBRARIAN : _float4(3.f, 0.2f, 50.f, 1.f);
	// SHOP : _float4(0.f, 17.f, 8.f, 1.f); _float4(0.f, 17.f, 38.f, 1.f);
	// PUZZLE : _float4(-0.2f, 0.02f, 51.f, 1.f);
	_float4 vPosition =  _float4(65.f, 2.f, -62.f, 1.f);
	m_vPrePosition = XMLoadFloat4(&vPosition);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pModelCom->Set_Animation_Index(ANIM_IDLE);
	m_pModelCom->Set_Animation_Transform(m_pTransformCom);
	Set_Animation();

	m_iMaxHP = m_iHP = 7;
	m_pUI_Stat = dynamic_cast<CUI_Stat*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Stat")));
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

	// TEST
	if (true == m_pGameInstance->Get_DIKeyState(DIK_B, KEY_DOWN))
	{
		if (DODGE_DASH == m_eDodge)
			m_eDodge = DODGE_ROLL;
		else if(DODGE_ROLL == m_eDodge)
			m_eDodge = DODGE_DASH;
	}

	Set_Dir();

	// UI_Obtain
	CUI_Obtain* pUIObtain = dynamic_cast<CUI_Obtain*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Obtain")));
	m_isObtain = pUIObtain->Get_Using();
	
	// State_Machine
	m_pModelCom->Update_State(fTimeDelta);
	if(false == m_isUsingInventory && false == m_isUsingShop && false == m_isObtain)
		Update_State();
	Update_Camera();

	// Change_Time
	m_fAccChageTime += fTimeDelta;
	if (false == m_isCanChange && m_fAccChageTime > m_fChangeTime)
	{
		m_isCanChange = true;
	}

	// Play_Animation
	if(false == m_isStop)
	{
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
	}

	// Navigation
	if(LEVEL_MENU != m_iLevel)
	{
		if (false == m_pNavigationCom->isMove(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION)))
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPrePosition);
		}
		m_vPrePosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

		// Height : 사다리 타는 중이 아닐 때만
		if (STATE_CLIMB != m_eState)
		{
			_float fHeight = m_pNavigationCom->Compute_Height(m_vPrePosition);
			if(false == isnan(fHeight))
			{
				m_vPrePosition.m128_f32[1] = fHeight;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPrePosition);
			}
		}
	}

	// Stat
	Compute_Stat_Gauge(fTimeDelta);

	// PartObject
	for (auto& PartObject : m_PartObjects)
		PartObject.second->Tick(fTimeDelta);

	// Inventory
	if (false == m_isUsingShop && true == m_pGameInstance->Get_DIKeyState(DIK_TAB, KEY_DOWN) && STATE_DODGE != m_eState)
	{	
		if (false == m_isObtain)
		{
			m_isUsingInventory = !m_isUsingInventory;
			m_pInventory->Set_Using(m_isUsingInventory);
			Change_State(STATE_IDLE);
		}
	}
	m_pInventory->Tick(fTimeDelta);

	// Interactive
	m_isInteractive = false;

	// Collider
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pRigidColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pGameInstance->Add_Group(CCollision_Manager::GROUP_PLAYER, this);
	m_pGameInstance->Add_RigidGroup(this);


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
		m_pGameInstance->Add_DebugComponent(m_pColliderCom);
		m_pGameInstance->Add_DebugComponent(m_pRigidColliderCom);
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
		 
		if (false == m_isInteractive && m_pGameInstance->Get_DIKeyState(DIK_SPACE, KEY_DOWN))
		{
			//m_eDodge = DODGE_ROLL;
			Change_State(STATE_DODGE);
		}

		if (m_pGameInstance->Get_DIKeyState(DIK_O, KEY_DOWN))
			Change_State(STATE_SLEEP);

		/*if (m_pGameInstance->Get_DIKeyState(DIK_K, KEY_DOWN))
			Change_State(STATE_DAMAGE);*/

		if (m_pGameInstance->Get_DIMouseState(DIMKS_RBUTTON, KEY_DOWN))
			Change_State(STATE_DEFENSE);

		if (m_pGameInstance->Get_DIKeyState(DIK_Z, KEY_DOWN))
			Change_State(STATE_PUZZLE);

		break;

	case STATE_PUZZLE:

		if (m_pGameInstance->Get_DIKeyState(DIK_Z, KEY_DOWN))
			Change_State(STATE_IDLE);

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


		if (m_pGameInstance->Get_DIMouseState(DIMKS_RBUTTON, KEY_DOWN))
			Change_State(STATE_DEFENSE);

		if (false == m_isInteractive && m_pGameInstance->Get_DIKeyState(DIK_SPACE, KEY_DOWN))
		{
			//m_eDodge = DODGE_ROLL;
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

	if (true == m_pGameInstance->Get_DIKeyState(DIK_X, KEY_DOWN))
		Change_State(STATE_CLIMB);
	if (true == m_pGameInstance->Get_DIKeyState(DIK_C, KEY_DOWN))
		Change_State(STATE_IDLE);
	if (true == m_pGameInstance->Get_DIKeyState(DIK_V, KEY_DOWN))
		Change_State(STATE_TOP);

	if (false == m_isUsingInventory)
	{
		if (m_pGameInstance->Get_DIKeyState(DIK_J, KEY_DOWN) ||
			m_pGameInstance->Get_DIKeyState(DIK_K, KEY_DOWN) ||
			m_pGameInstance->Get_DIKeyState(DIK_L, KEY_DOWN))
		{
			if (WEAPON_END != m_eWeapon)
			{
				CPlayer_Weapon* pWeapon = Find_Weapon(m_eWeapon);
				if (nullptr == pWeapon)
					return;

				pWeapon->Set_isUsing(false);
			}

			if (m_pGameInstance->Get_DIKeyState(DIK_J, KEY_DOWN))
			{				
				m_eWeapon = (WEAPON)(m_pInventory->Get_Weapon(DIK_J));
				if (WEAPON_END != m_eWeapon)
				{
					Set_Weapon(DIK_J);
				}
			}
			else if (m_pGameInstance->Get_DIKeyState(DIK_K, KEY_DOWN))
			{
				m_eWeapon = (WEAPON)(m_pInventory->Get_Weapon(DIK_K));
				if (WEAPON_END != m_eWeapon)
				{
					Set_Weapon(DIK_K);
				}
			}
			else if (m_pGameInstance->Get_DIKeyState(DIK_L, KEY_DOWN))
			{
				m_eWeapon = (WEAPON)(m_pInventory->Get_Weapon(DIK_L));
				if (WEAPON_END != m_eWeapon)
				{
					Set_Weapon(DIK_L);
				}
			}

			if(false == isAttack())
			{
				switch (m_eWeapon)
				{
				case WEAPON_STICK:
					Change_State(STATE_ATTACK_STICK);
					break;
				case WEAPON_SWORD:
					Change_State(STATE_ATTACK_SWORD);
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
		}

		if (true == m_pGameInstance->Get_DIKeyState(DIK_SEMICOLON, KEY_DOWN))
		{
			if(true == m_pInventory->Get_HaveItem(CItem::ITEM_SHIELD))
				Change_State(STATE_DEFENSE);
		}
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

HRESULT CPlayer::Set_Navigation(_uint iLevel)
{
	/* For.Com_Navigation */
	if(nullptr != m_pNavigationCom)
	{
		Safe_Release(m_pNavigationCom);
		Delete_Component(TEXT("Com_Navigation"));
	}

	CNavigation::NAVIGATION_DESC			NavigationDesc{};
	NavigationDesc.iCurrentIndex = 0;
	if (FAILED(__super::Add_Component(iLevel, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
		return E_FAIL;

	_float4 vPosition{};

	switch (iLevel)
	{
	case LEVEL_BEACH:
		vPosition = _float4(65.f, 2.f, -62.f, 1.f);
		break;
	case LEVEL_SHOP:
		vPosition = _float4(0.f, 17.f, 8.f, 1.f);
		break;
	case LEVEL_PUZZLE:
		vPosition = _float4(-0.2f, 0.02f, -51.f, 1.f);
		break;
	case LEVEL_BOSS:
		vPosition = _float4(0.f, 0.2f, 54.f, 1.f);
		break;
	default:
		break;
	}

	m_vPrePosition = XMLoadFloat4(&vPosition);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

void CPlayer::Set_Weapon_Render(const wstring& strWeaponTag, _bool isRender)
{
	CPartObject* pWeapon = m_PartObjects.find(strWeaponTag)->second;
	if (nullptr == pWeapon)
		return;

	dynamic_cast<CPlayer_Weapon*>(pWeapon)->Set_isUsing(isRender);
}

void CPlayer::Set_UtileItem(WEAPON eWeapon)
{
	CPlayer_Weapon* pWeapon = Find_Weapon(eWeapon);
	if (nullptr == pWeapon)
		return;

	pWeapon->Set_isUsing(true);
}

HRESULT CPlayer::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strModelComTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For. Com_Collider */
	/*CBounding_SPHERE::BOUNDING_SPHERE_DESC ColliderDesc{};
	
	ColliderDesc.fRadius = 0.8f;
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius + 0.6f , 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;*/

	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};
	ColliderDesc.vSize = _float3(1.7f, 2.f, 1.7f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For. Com_RigidCollider */
	CBounding_OBB::BOUNDING_OBB_DESC		RigidDesc{};

	// 로컬상의 정보를 셋팅한다.
	RigidDesc.vSize = _float3(1.7f, 2.f, 1.7f);
	RigidDesc.vCenter = _float3(0.f, RigidDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_RigidCollider"), (CComponent**)&m_pRigidColliderCom, &RigidDesc)))
		return E_FAIL;

	///* For.Com_Navigation */
	//CNavigation::NAVIGATION_DESC			NavigationDesc{};
	//NavigationDesc.iCurrentIndex = 0;
	//if (FAILED(__super::Add_Component(LEVEL_BEACH, TEXT("Prototype_Component_Navigation"),
	//	TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
	//	return E_FAIL;
	
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

	/* For. Part_Player_Weapon_Dash */
	pWeaponObject = { nullptr };
	tDesc = {};

	tDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	tDesc.pSocketBone = m_pModelCom->Get_Bone_Ptr(DASHBONE);
	tDesc.eWeapon = WEAPON_DASH;
	pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Part_Player_Weapon_Dash"), &tDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;

	m_PartObjects.emplace(TEXT("Part_Player_Weapon_Dash"), pWeaponObject);

	return S_OK;
}

HRESULT CPlayer::Add_States()
{
	m_pModelCom->Add_State(STATE_IDLE, CPlayer_State_Idle::Create(this));
	m_pModelCom->Add_State(STATE_SLEEP, CPlayer_State_Sleep::Create(this));
	m_pModelCom->Add_State(STATE_MOVE, CPlayer_State_Move::Create(this));
	m_pModelCom->Add_State(STATE_ATTACK_STICK, CPlayer_State_Attack_Stick::Create(this, dynamic_cast<CPlayer_Weapon*>(m_PartObjects.find(TEXT("Part_Player_Weapon_Stick"))->second)));
	m_pModelCom->Add_State(STATE_ATTACK_SWORD, CPlayer_State_Attack_Sword::Create(this, dynamic_cast<CPlayer_Weapon*>(m_PartObjects.find(TEXT("Part_Player_Weapon_Sword"))->second)));
	m_pModelCom->Add_State(STATE_ATTACK_WAND, CPlayer_State_Attack_Wand::Create(this, dynamic_cast<CPlayer_Weapon*>(m_PartObjects.find(TEXT("Part_Player_Weapon_Wand"))->second)));
	m_pModelCom->Add_State(STATE_DAMAGE, CPlayer_State_Damage::Create(this));
	m_pModelCom->Add_State(STATE_DODGE, CPlayer_State_Dodge::Create(this));
	m_pModelCom->Add_State(STATE_DEFENSE, CPlayer_State_Defense::Create(this));
	m_pModelCom->Add_State(STATE_OPEN, CPlayer_State_Open::Create(this));
	m_pModelCom->Add_State(STATE_PUZZLE, CPlayer_State_Puzzle::Create(this));
	m_pModelCom->Add_State(STATE_CLIMB, CPlayer_State_Climb::Create(this));
	m_pModelCom->Add_State(STATE_TOP, CPlayer_State_Top::Create(this));
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
	m_pModelCom->Set_Animation_isLoop(ANIM_CLIMB, true);
	m_pModelCom->Set_Animation_isLoop(ANIM_FALLING, true);

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
	m_pModelCom->Set_Animation_isRoot(ANIM_CLIMB_ON, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_CLIMB_OFF, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_CLIMB, true);

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
	m_pModelCom->Set_Frame_Tick(ANIM_HYPERDASH, 0, 2, 5.f);
	//m_pModelCom->Set_Frame_Tick(ANIM_SWING_STICK2, 36, 39, 80.f);
	m_pModelCom->Set_Frame_Tick(ANIM_SWING_SWORD3, 60, 67, 50.f);

	//m_pModelCom->Set_Frame_Tick(ANIM_SWING_STICK1, 2, 55, 1.3f);
	//m_pModelCom->Set_Frame_Tick(ANIM_SWING_STICK2, 2, 36, 1.3f);

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

void CPlayer::Set_Weapon(_uint iKey)
{
	CPlayer_Weapon* pWeapon = Find_Weapon(m_eWeapon);
	if (nullptr == pWeapon)
		return;

	pWeapon->Set_Key(iKey);

	pWeapon->Set_isUsing(true);
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

void CPlayer::Compute_Height()
{
	/*matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);*/


	//_matrix matWVP = /*m_pTransformCom->Get_WorldMatrix() * */m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW) * m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);

	//_vector vPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	//vPosition = XMVector3TransformCoord(vPosition, matWVP);

	//_float2 vProjPosition = { (XMVectorGetX(vPosition) / XMVectorGetW(vPosition) + 1.f) * 0.5f * 1280.f,
	//							(1.f - XMVectorGetY(vPosition) / XMVectorGetW(vPosition)) * 0.5f * 720.f };

	//vPosition = m_pGameInstance->Compute_WorldPos(vProjPosition, TEXT("Target_FieldDepth"));

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	_vector vWorldPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	//static _float fY = vWorldPosition.m128_f32[1];
	//vWorldPosition.m128_f32[1] = fY;
	_vector vViewPos = XMVector3TransformCoord(vWorldPosition, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
	_vector vProjPos = XMVector3TransformCoord(vViewPos, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

	// -1 ~ 1 -> 0 ~ 1
	/*_float2 vProjPosition = { (XMVectorGetX(vProjPos) / XMVectorGetW(vProjPos) + 1.f) * 0.5f * 1280.f,
								(1.f - XMVectorGetY(vProjPos) / XMVectorGetW(vProjPos)) * 0.5f * 720.f };*/

	
	_float2 vProjPosition = { (XMVectorGetX(vProjPos) + 1.f) * 0.5f * 1280.f,
								(1.f - XMVectorGetY(vProjPos)) * 0.5f * 720.f };

	_vector vPosition = m_pGameInstance->Compute_WorldPos(vProjPosition, TEXT("Target_FieldDepth"));
	//vPosition.m128_f32[1] += 0.2f;
	//fY = vPosition.m128_f32[1];
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);


	/*_vector		vWorldPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	_float2		vMousePos = _float2(ptMouse.x, ptMouse.y);

	vWorldPos = m_pGameInstance->Compute_WorldPos(vMousePos, TEXT("Target_FieldDepth"));*/

	/*CParticle_Red::PARTICLE_DESC tDesc{};
	tDesc.vPosition = vWorldPos;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Red"), &tDesc)))
		return;*/
}

CPlayer_Weapon* CPlayer::Find_Weapon(WEAPON eWeapon)
{
	for (auto iter = m_PartObjects.begin(); iter != m_PartObjects.end(); ++iter)
	{
		CPlayer_Weapon* pWeapon = dynamic_cast<CPlayer_Weapon*>(iter->second);
		if (pWeapon->Get_Weapon() == eWeapon)
			return pWeapon;
	}

	return nullptr;
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
	Safe_Release(m_pRigidColliderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pLookOnTransform);

	Safe_Release(m_pUI_Stat);
	Safe_Release(m_pInventory);
}

void CPlayer::Collision_Event(Engine::CGameObject* pGameObject)
{
	// 상호작용 물체 && 스페이스 바 를 눌렀을 때
	if (OBJ_INTERACTIVE == pGameObject->Get_ObjectType())
	{
		m_isInteractive = true;

		if(true == m_pGameInstance->Get_DIKeyState(DIK_SPACE, KEY_DOWN) || true == m_isChestOpen || true == m_isUsingShop || STATE_CLIMB == m_eState)
		{
			CInteractiveObject* pObject = dynamic_cast<CInteractiveObject*>(pGameObject);

			// 상자
			CInteractiveObject::INTERACTIVE eInteractiveType = pObject->Get_InteractiveType();
			if (CInteractiveObject::INTERACTIVE_CHEST == eInteractiveType)
			{
				CObject_Chest* pChest = dynamic_cast<CObject_Chest*>(pObject);
				// 아직 열지 않은 상태일 때 : 열고 아이템을 얻음
				if (true == pChest->Get_isClose())
				{
					Change_State(STATE_OPEN);
					if(true == m_isChestOpen)
					{
						CItem* pItem = pChest->Set_Open();
						m_pInventory->Add_Item(pItem);
						m_isChestOpen = false;

						// 방패
						CItem::ITEM eItem = pItem->Get_Item();
						if (CItem::ITEM_SHIELD == eItem)
							Set_UtileItem(WEAPON_SHIELD);
					}
				}
			}
			else if (CInteractiveObject::INTERACTIVE_ITEM == eInteractiveType)
			{
				m_isUsingShop = true;
				CItem* pItem = dynamic_cast<CItem*>(pObject);
				pItem->Select_Item();
			
				if (true == m_isUsingShop && true == m_pGameInstance->Get_DIKeyState(DIK_RETURN, KEY_DOWN))
				{
					// 아이템 샀을 때
					if (true == pItem->Get_isOK())
					{
						CItem* pItemBuy = pItem->Buy_Item(m_pInventory->Get_NumCubic());
						if (nullptr == pItemBuy)
							return;
						m_pInventory->Add_Item(pItemBuy);

						// 대쉬
						CItem::ITEM eItem = pItem->Get_Item();
						if (CItem::ITEM_DASH == eItem)
						{
							Set_UtileItem(WEAPON_DASH);
							m_eDodge = DODGE_DASH;
						}
					}
					// 취소 했을 때
					else
					{
						pItem->Exit_Shop();
					}

					Change_State(STATE_IDLE);
					m_isUsingShop = false;
				}
			}
			else if (CInteractiveObject::INTERACTIVE_LADDER == eInteractiveType)
			{
				CObject_Ladder* pLadder = dynamic_cast<CObject_Ladder*>(pObject);
				m_isUpper = pLadder->Get_isUpper();
				if(STATE_CLIMB != m_eState)
					Change_State(STATE_CLIMB);
				// 등산 상태일 때
				else if (false == m_isArrive && STATE_CLIMB == m_eState)
				{
					// 올라가고 있었는데 내려가는 충돌 박스를 만났을 때 : 다 올라옴
					if (CLIMB_UPPER == m_eClimb && false == m_isUpper)
					{
						m_isArrive = true;
						m_iLadderIndex = pLadder->Get_Index();

						if (true == pLadder->Get_isEnd())
							m_isEndLadder = true;
					}
					// 내려가고 있었는데 올라가는 충돌 박스를 만났을 때 : 다 내려옴
					else if (CLIMB_LOWER == m_eClimb && true == m_isUpper)
					{
						m_isArrive = true;
						m_iLadderIndex = pLadder->Get_Index();
					}
				}
			}
			else if (CInteractiveObject::INTERACTIVE_TELESCOPE == eInteractiveType)
			{
				m_pGameInstance->Change_Camera(TEXT("Camera_Telescope"));
				Change_State(STATE_IDLE);
			}
		}
	}

}

void CPlayer::Damage_Event()
{
}
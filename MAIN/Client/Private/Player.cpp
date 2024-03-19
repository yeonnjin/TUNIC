#include "stdafx.h"
#include "Player.h"

#include "Player_Weapon.h"

// State
#include "Player_State_Idle.h"
#include "Player_State_Sleep.h"
#include "Player_State_Move.h"
#include "Player_State_Attack_Stick.h"
#include "Player_State_Damage.h"
#include "Player_State_Dodge.h"
#include "Player_State_Defense.h"

#define	WEAPONBONEIDX 24
// stick - 29 / sword - 45 / shield - 24

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

	GameObjectDesc.fSpeedPerSec = 3.f;
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

	_float4 vPosition = _float4(0.f, 0.5f, 0.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pModelCom->Set_Animation_Index(ANIM_WALK_LEFT);
	m_pModelCom->Set_Animation_Transform(m_pTransformCom);
	Set_Animation_Loop();

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	//static _uint iIndex = 0;
	//if (m_pGameInstance->Get_DIKeyState(DIK_Z, KEY_DOWN))
	//{
	//	iIndex++;
	//	if (iIndex > 60)
	//		iIndex = 0;

	//	if (iIndex == 39)
	//	{
	//		int a = 0;
	//	}

	//	//m_pModelCom->Set_Animation_Index(iIndex);
	//	m_isBlend = true;
	//}
	//if (m_pGameInstance->Get_DIKeyState(DIK_X, KEY_DOWN))
	//{
	//	iIndex--;
	//	if (iIndex < 0)
	//		iIndex = 60;

	//	//m_pModelCom->Set_Animation_Index(iIndex);
	//	m_isBlend = true;
	//}

	//if (m_pGameInstance->Get_DIKeyState(DIK_UP, KEY_PRESS))
	//{
	//	m_pTransformCom->Go_Straight(fTimeDelta);
	//}
	//if (m_pGameInstance->Get_DIKeyState(DIK_LEFT, KEY_PRESS))
	//{
	//	m_pTransformCom->Go_Left(fTimeDelta);
	//}
	//if (m_pGameInstance->Get_DIKeyState(DIK_DOWN, KEY_PRESS))
	//{
	//	m_pTransformCom->Go_Backward(fTimeDelta);
	//}
	//if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT, KEY_PRESS))
	//{
	//	m_pTransformCom->Go_Right(fTimeDelta);
	//}

	//// Test
	////m_isBlend = false;
	//if (true == m_pModelCom->isFinished())
	//{
	//	m_isBlend = true;
	//}

	//if (true == m_isBlend)
	//{
	//	if (S_OK == m_pModelCom->Blending_Animation(iIndex, fTimeDelta))
	//	{
	//		m_isBlend = false;
	//		m_pModelCom->Set_Animation_Index(iIndex);
	//	}
	//}
	
	// PartObject
	for (auto& PartObject : m_PartObjects)
		PartObject.second->Tick(fTimeDelta);

	// State_Machine
	m_pModelCom->Update_State(fTimeDelta);
	Update_State();


	// Blending
	if (true == m_isBlend)
	{
		if (m_eAnimationIndex == ANIM_WALK_LEFT && m_eBlendAnimIndex == ANIM_WALK_LEFT)
		{
			int a = 0;
		}

		if (S_OK == m_pModelCom->Blending_Animation(m_eBlendAnimIndex, fTimeDelta))
		{
			m_isBlend = false;
			m_pModelCom->Set_Animation_Index(m_eBlendAnimIndex);
			m_eAnimationIndex = m_eBlendAnimIndex;
		}
	}	
	/*else
		m_pModelCom->Play_Animation(fTimeDelta);*/

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	for (auto& PartObject : m_PartObjects)
		PartObject.second->Late_Tick(fTimeDelta);

	if (false == m_isBlend)
		m_pModelCom->Play_Animation(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG


	return S_OK;
}

void CPlayer::Update_State()
{
	static _bool isTurn = false;

	switch (m_eState)
	{
	case STATE_IDLE:
		// IDLE -> MOVE
		if (m_pGameInstance->Get_DIKeyState(DIK_W, KEY_PRESS))
		{
			m_eDir = DIR_FORWARD;			
			Change_State(STATE_MOVE);
		}
		else if (m_pGameInstance->Get_DIKeyState(DIK_S, KEY_PRESS))
		{
			m_eDir = DIR_BACKWARD;
			Change_State(STATE_MOVE);
		}
		else if (m_pGameInstance->Get_DIKeyState(DIK_A, KEY_PRESS))
		{
			m_eDir = DIR_LEFT;
			Change_State(STATE_MOVE);
		}
		else if (m_pGameInstance->Get_DIKeyState(DIK_D, KEY_PRESS))
		{
			m_eDir = DIR_RIGHT;
			Change_State(STATE_MOVE);
		}

		if (m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON, KEY_DOWN))
			Change_State(STATE_ATTACK_STICK);

		if (m_pGameInstance->Get_DIKeyState(DIK_O, KEY_DOWN))
			Change_State(STATE_SLEEP);

		if (m_pGameInstance->Get_DIKeyState(DIK_K, KEY_DOWN))
			Change_State(STATE_DAMAGE);

		if (m_pGameInstance->Get_DIKeyState(DIK_SPACE, KEY_DOWN))
			Change_State(STATE_DODGE);

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
	
		if (m_pGameInstance->Get_DIKeyState(DIK_SPACE, KEY_DOWN))
			Change_State(STATE_DODGE);

		if (m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON, KEY_DOWN))
			Change_State(STATE_ATTACK_STICK);

		if (m_pGameInstance->Get_DIMouseState(DIMKS_RBUTTON, KEY_DOWN))
			Change_State(STATE_DEFENSE);

		break;
	case STATE_END:
		break;
	default:
		break;
	}
}

void CPlayer::Change_State(STATE eState)
{
	m_pModelCom->Change_State(eState);
	m_eState = eState;
}

void CPlayer::Set_Weapon_Render(const wstring& strWeaponTag, _bool isRender)
{
	CPartObject* pWeapon = m_PartObjects.find(strWeaponTag)->second;
	if (nullptr == pWeapon)
		return;

	dynamic_cast<CPlayer_Weapon*>(pWeapon)->Set_isRender(isRender);
}

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

	return S_OK;
}

HRESULT CPlayer::Add_PartObjects()
{
	/* For. Part_Player_Weapon */
	CPartObject* pWeaponObject = { nullptr };
	CPlayer_Weapon::PLAYER_WEAPON_DESC tDesc{};

	CAnimator* pModel = m_pModelCom;

	tDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	tDesc.pSocketBone = pModel->Get_Bone_Ptr(WEAPONBONEIDX);

	pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Part_Player_Weapon"), &tDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;

	m_PartObjects.emplace(TEXT("Part_Player_Weapon"), pWeaponObject);

	return S_OK;
}

HRESULT CPlayer::Add_States()
{
	m_pModelCom->Add_State(STATE_IDLE, CPlayer_State_Idle::Create(this));
	m_pModelCom->Add_State(STATE_SLEEP, CPlayer_State_Sleep::Create(this));
	m_pModelCom->Add_State(STATE_MOVE, CPlayer_State_Move::Create(this));
	m_pModelCom->Add_State(STATE_ATTACK_STICK, CPlayer_State_Attack_Stick::Create(this));
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

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Set_Animation_Loop()
{
	m_pModelCom->Set_Animation_isLoop(ANIM_IDLE, true);
	m_pModelCom->Set_Animation_isLoop(ANIM_SLEEPING, true);
	//m_pModelCom->Set_Animation_isLoop(ANIM_GETUP, true);
	m_pModelCom->Set_Animation_isLoop(ANIM_WALK_FORWARD, true);
	m_pModelCom->Set_Animation_isLoop(ANIM_WALK_BACKWARD, true);
	m_pModelCom->Set_Animation_isLoop(ANIM_WALK_LEFT, true);
	m_pModelCom->Set_Animation_isLoop(ANIM_WALK_RIGHT, true);
	m_pModelCom->Set_Animation_isLoop(ANIM_SHIELD, true);
	//m_pModelCom->Set_Animation_isLoop(ANIM_SWING_STICK1, true);
	//m_pModelCom->Set_Animation_isLoop(ANIM_SWING_STICK2, true);

	m_pModelCom->Set_Animation_isRoot(ANIM_SWING_STICK1, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_SWING_STICK2, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_HURT, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_STAGGER, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_DODGE, true);

	m_pModelCom->Set_Blend_Time(ANIM_SWING_STICK1, 0.1f);
	m_pModelCom->Set_Blend_Time(ANIM_SWING_STICK2, 0.1f);
	m_pModelCom->Set_Blend_Time(ANIM_DODGE, 0.3f);
	m_pModelCom->Set_Blend_Time(ANIM_STAGGER, 0.4f);
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
}
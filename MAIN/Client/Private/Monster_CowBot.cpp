#include "stdafx.h"
#include "Monster_CowBot.h"
#include "CowBot_Weapon.h"

#include "CowBot_State_Idle.h"
#include "CowBot_State_Walk.h"
#include "CowBot_State_Run.h"
#include "CowBot_State_Attack.h"
#include "CowBot_State_Damage.h"
#include "CowBot_State_Die.h"

#include "Player.h"

#define SWORDBONE 21
#define SHIELDBONE 18

CMonster_CowBot::CMonster_CowBot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CMonster_CowBot::CMonster_CowBot(const CMonster_CowBot& rhs)
	: CMonster{ rhs }
{
}

void CMonster_CowBot::Change_State(STATE eState)
{
	m_pModelCom->Change_State(eState);
	m_eState = eState;
}

HRESULT CMonster_CowBot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_CowBot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_States()))
		return E_FAIL;

	_float4 vPosition = _float4(18.f, 2.5f, 83.f, 1.f);
	//_float4 vPosition = _float4(-2.f + rand() % 6, 0.5f, rand() % 6 + 5.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_pModelCom->Set_Animation_Index(ANIM_IDLE);
	m_pModelCom->Set_Animation_Transform(m_pTransformCom);
	Set_Animation();

	m_iHP = 3;

	return S_OK;
}

HRESULT CMonster_CowBot::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	/*static _uint iIndex = 0;
	  if (m_pGameInstance->Get_DIKeyState(DIK_I, KEY_DOWN))
	  {
		  iIndex++;
		  if (iIndex > 10)
			  iIndex = 0;
		  m_pModelCom->Set_Animation_Index(iIndex);
	  }

	  m_pModelCom->Play_Animation(fTimeDelta);*/
	// PartObject
	for (auto& PartObject : m_PartObjects)
		PartObject.second->Tick(fTimeDelta);

	return S_OK;
}

void CMonster_CowBot::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	for (auto& PartObject : m_PartObjects)
		PartObject.second->Late_Tick(fTimeDelta);
}

HRESULT CMonster_CowBot::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_CowBot::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	/* For. Com_Collider */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC ColliderDesc{};

	ColliderDesc.fRadius = 1.6f;
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_CowBot::Add_PartObjects()
{
	/* For. Part_CowBot_Weapon_Sword */
	CPartObject* pWeaponObject = { nullptr };
	CCowBot_Weapon::COWBOT_WEAPON_DESC tDesc{};

	tDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	tDesc.pSocketBone = m_pModelCom->Get_Bone_Ptr(SWORDBONE);
	_char szModelTag[MAX_PATH] = "Prototype_Component_Model_Monster_CowBot_Sword";
	wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
	tDesc.strModelComTag = wstr;
	tDesc.eWeapon = WEAPON_SWORD;

	pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Part_CowBot_Weapon"), &tDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;

	m_PartObjects.emplace(TEXT("Part_CowBot_Weapon_Sword"), pWeaponObject);

	/* For. Part_CowBot_Weapon_Shield */
	pWeaponObject = { nullptr };
	tDesc = {};

	tDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	tDesc.pSocketBone = m_pModelCom->Get_Bone_Ptr(SHIELDBONE);
	_char szModelTag1[MAX_PATH] = "Prototype_Component_Model_Monster_CowBot_Shield";
	wstring wstr1(&szModelTag1[0], &szModelTag1[MAX_PATH]);
	tDesc.strModelComTag = wstr1;
	tDesc.eWeapon = WEAPON_SHIELD;

	pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Part_CowBot_Weapon"), &tDesc));
	if (nullptr == pWeaponObject)
		return E_FAIL;

	m_PartObjects.emplace(TEXT("Part_CowBot_Weapon_Shield"), pWeaponObject);

	return S_OK;
}

HRESULT CMonster_CowBot::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_CowBot::Add_States()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0));
	m_pModelCom->Add_State(STATE_IDLE, CCowBot_State_Idle::Create(this, pPlayer));
	m_pModelCom->Add_State(STATE_WALK, CCowBot_State_Walk::Create(this, pPlayer));
	m_pModelCom->Add_State(STATE_RUN, CCowBot_State_Run::Create(this, pPlayer));
	m_pModelCom->Add_State(STATE_DAMAGE, CCowBot_State_Damage::Create(this, pPlayer));
	m_pModelCom->Add_State(STATE_DIE, CCowBot_State_Die::Create(this, pPlayer));

	CCowBot_Weapon* pWeapon = dynamic_cast<CCowBot_Weapon*>(m_PartObjects.find(TEXT("Part_CowBot_Weapon_Sword"))->second);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_pModelCom->Add_State(STATE_ATTACK, CCowBot_State_Attack::Create(this, pPlayer, pWeapon));

	m_pModelCom->Change_State(STATE_IDLE);
	m_eState = STATE_IDLE;

	return S_OK;
}

void CMonster_CowBot::Update_State()
{
}

void CMonster_CowBot::Set_Animation()
{
	// LOOP
	m_pModelCom->Set_Animation_isLoop(ANIM_IDLE, true);
	m_pModelCom->Set_Animation_isLoop(ANIM_RUN, true);
	m_pModelCom->Set_Animation_isLoop(ANIM_WALK, true);

	// ROOT
	m_pModelCom->Set_Animation_isRoot(ANIM_RUN, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_WALK, true);
	m_pModelCom->Set_Animation_isRoot(ANIM_ATTACK, true);

	// BLEND TIME
	//m_pModelCom->Set_Blend_Time(ANIM_ATTACK, 1.f);
	//m_pModelCom->Set_Blend_Time(ANIM_BACKSWIPE, 0.3f);
}

CMonster_CowBot* CMonster_CowBot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_CowBot* pInstance = new CMonster_CowBot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CMonster_CowBot"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_CowBot::Clone(void* pArg)
{
	CMonster_CowBot* pInstance = new CMonster_CowBot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CMonster_CowBot"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_CowBot::Free()
{
	__super::Free();

	for (auto& PartObject : m_PartObjects)
		Safe_Release(PartObject.second);

	m_PartObjects.clear();
}

void CMonster_CowBot::Collision_Event(Engine::CGameObject* pGameObject)
{
}

void CMonster_CowBot::Damage_Event()
{
	if (0 >= m_iHP)
		Change_State(STATE_DIE);
	else
		Change_State(STATE_DAMAGE);
}

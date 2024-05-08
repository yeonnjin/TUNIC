#include "stdafx.h"
#include "Monster_Librarian.h"
#include "Librarian_Weapon.h"

#include "Librarian_State_Pattern_Energy_Wave.h"
#include "Librarian_State_Pattern_Lightning_Warp.h"
#include "Librarian_State_Pattern_Lunge_Swipe.h"
#include "Librarian_State_Pattern_Homing_Orbs.h"
#include "Librarian_State_Pattern_Energy_Beam.h"
#include "Librarian_State_Pattern_Summon_Bat.h"

#include "Librarian_State_Entry.h"
#include "Librarian_State_Idle.h"
#include "Librarian_State_Melee.h"
#include "Librarian_State_Damage.h"
#include "Librarian_State_Die.h"

#include "Player.h"

#include "UI_BossHP.h"

#define SWORDBONE 94
#define SHIELDBONE 18

CMonster_Librarian::CMonster_Librarian(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CMonster_Librarian::CMonster_Librarian(const CMonster_Librarian& rhs)
    : CMonster{ rhs }
{
}

_float4 CMonster_Librarian::Get_Bone_Position(_uint iBoneIndex)
{
    return m_pModelCom->Get_Bone_Position(iBoneIndex);
}

_uint CMonster_Librarian::Get_Pattern()
{
    _uint iPattern = m_States.front();
    m_States.pop();

    if (true == m_States.empty()) 
        Add_Patterns();

    return iPattern;
}

void CMonster_Librarian::Change_State(STATE eState)
{
    m_pModelCom->Change_State(eState);
    m_eState = eState;
}

HRESULT CMonster_Librarian::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster_Librarian::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;

    if (FAILED(Add_States()))
        return E_FAIL;

    if (FAILED(Add_Patterns()))
        return E_FAIL;

    //m_pModelCom->Set_Animation_Index(ANIM_FLYING_IDLE);
    m_pModelCom->Set_Animation_Transform(m_pTransformCom);
    Set_Animation();

    _float4 vPosition = _float4(-4.8f, 0.1f, -6.5f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    m_iHP = 7;
    m_fDamageCoolTime = 0.2f;
    m_eRigid = RIGID_BLOCK;

    m_pUIBossHP = dynamic_cast<CUI_BossHP*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS, TEXT("Layer_UI_BossHP")));
    if (nullptr == m_pUIBossHP)
        return E_FAIL;

    //Safe_AddRef(m_pUIBossHP);

    m_pUIBossHP->Set_MaxHp(m_iHP);

    return S_OK;
}

HRESULT CMonster_Librarian::Tick(_float fTimeDelta)
{
   if (true == m_isDead)
        return E_FAIL;

   // State_Machine
   m_pModelCom->Update_State(fTimeDelta);
   Update_State();

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

    for (auto& PartObject : m_PartObjects)
        PartObject.second->Tick(fTimeDelta);

    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    m_pRigidColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    m_pGameInstance->Add_Group(CCollision_Manager::GROUP_MONSTER, this);

    m_pGameInstance->Add_RigidGroup(this);

    if(true == m_pGameInstance->Get_DIKeyState(DIK_NUMPAD2, KEY_DOWN))
        m_pUIBossHP->Set_HPMinus();

    return S_OK;
}

void CMonster_Librarian::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    for (auto& PartObject : m_PartObjects)
        PartObject.second->Late_Tick(fTimeDelta);
}

HRESULT CMonster_Librarian::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Librarian::Add_Components()
{
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For. Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC ColliderDesc{};

    ColliderDesc.vSize = _float3(1.8f, 8.f, 1.8f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    /* For.Com_Navigation */
    CNavigation::NAVIGATION_DESC			NavigationDesc{};
    NavigationDesc.iCurrentIndex = 40;
    if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Librarian::Add_PartObjects()
{
    /* For. Part_Librarian_Weapon_Sword */
    CPartObject* pWeaponObject = { nullptr };
    CLibrarian_Weapon::LIBRARIAN_WEAPON_DESC tDesc{};

    tDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
    tDesc.pSocketBone = m_pModelCom->Get_Bone_Ptr(SWORDBONE);
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Boss_Librarian_Sword";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    tDesc.strModelComTag = wstr;
    tDesc.eWeapon = WEAPON_SWORD;

    pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Part_Librarian_Weapon"), &tDesc));
    if (nullptr == pWeaponObject)
        return E_FAIL;

    m_PartObjects.emplace(TEXT("Part_Librarian_Weapon_Sword"), pWeaponObject);

    return S_OK;
}

HRESULT CMonster_Librarian::Add_Patterns()
{
    // 에너지 웨이브
    m_States.push(STATE_PATTERN_ENERGY_WAVE);
    // 번개
    m_States.push(STATE_PATTERN_LIGHTNING_WARP);
    // 박쥐 소환
    m_States.push(STATE_PATTERN_SUMMON_BAT);
    // 런지 
    m_States.push(STATE_PATTERN_LUNGE_SWIPE);
    // 에너지 빔
    m_States.push(STATE_PATTERN_ENERGY_BEAM);
    // 오브 소환
    m_States.push(STATE_PATTERN_HOMING_ORBS);

    return S_OK;
}

HRESULT CMonster_Librarian::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Librarian::Add_States()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0));
    m_pModelCom->Add_State(STATE_PATTERN_LIGHTNING_WARP, CLibrarian_State_Pattern_Lightning_Warp::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_PATTERN_ENERGY_WAVE, CLibrarian_State_Pattern_Energy_Wave::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_PATTERN_LUNGE_SWIPE, CLibrarian_State_Pattern_Lunge_Swipe::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_PATTERN_HOMING_ORBS, CLibrarian_State_Pattern_Homing_Orbs::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_PATTERN_ENERGY_BEAM, CLibrarian_State_Pattern_Energy_Beam::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_PATTERN_SUMMON_BAT, CLibrarian_State_Pattern_Summon_Bat::Create(this, pPlayer));

    m_pModelCom->Add_State(STATE_ENTRY, CLibrarian_State_Entry::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_IDLE, CLibrarian_State_Idle::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_MELEE, CLibrarian_State_Melee::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_DAMAGE, CLibrarian_State_Damage::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_DIE, CLibrarian_State_Die::Create(this, pPlayer));

    m_pModelCom->Change_State(STATE_ENTRY);
    m_eState = STATE_ENTRY;

    return S_OK;
}

void CMonster_Librarian::Update_State()
{
    switch (m_eState)
    {
    case STATE_IDLE:
        if (m_pGameInstance->Get_DIKeyState(DIK_1, KEY_DOWN))
            Change_State(STATE_PATTERN_LIGHTNING_WARP);
        else if (m_pGameInstance->Get_DIKeyState(DIK_2, KEY_DOWN))
            Change_State(STATE_PATTERN_ENERGY_WAVE);
        else if (m_pGameInstance->Get_DIKeyState(DIK_3, KEY_DOWN))
            Change_State(STATE_PATTERN_LUNGE_SWIPE);
        else if (m_pGameInstance->Get_DIKeyState(DIK_4, KEY_DOWN))
            Change_State(STATE_PATTERN_HOMING_ORBS);
        else if (m_pGameInstance->Get_DIKeyState(DIK_5, KEY_DOWN))
            Change_State(STATE_PATTERN_ENERGY_BEAM);
        else if (m_pGameInstance->Get_DIKeyState(DIK_6, KEY_DOWN))
            Change_State(STATE_PATTERN_SUMMON_BAT);
        else if (m_pGameInstance->Get_DIKeyState(DIK_7, KEY_DOWN))
            Change_State(STATE_MELEE);
        break;

    default:
        break;
    }

}

void CMonster_Librarian::Set_Animation()
{
    /*for(_uint i = 0; i < 35; ++i)
        m_pModelCom->Set_Animation_isLoop(i, true);*/

    // LOOP
    m_pModelCom->Set_Animation_isLoop(ANIM_FLYING_IDLE, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_FLYING_IDLE_LEFT, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_FLYING_IDLE_RIGHT, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_ENTRY, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_LIGHTNING_LOOP, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_SUMMON_ORB_LOOP, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_FLYING_SWOOP, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_PALPATINE_LOOP, true);

    // ROOT
    m_pModelCom->Set_Animation_isRoot(ANIM_DIE, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_FAST_SLAM, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_LIGHTNING_WAIT, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_LIGHTNING_RELEASE, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_LIGHTNING_WINDUP, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_LIGHTNING_LAND, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_FLYING_SWOOP, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_SUMMON_ORB_LOOP, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_PALPATINE_START, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_TURN, true);  
}

CMonster_Librarian* CMonster_Librarian::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster_Librarian* pInstance = new CMonster_Librarian(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CMonster_Librarian"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster_Librarian::Clone(void* pArg)
{
    CMonster_Librarian* pInstance = new CMonster_Librarian(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CMonster_Librarian"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMonster_Librarian::Free()
{
    __super::Free();

    for (auto& PartObject : m_PartObjects)
        Safe_Release(PartObject.second);

    m_PartObjects.clear();

    //Safe_Release(m_pUIBossHP);
}

void CMonster_Librarian::Collision_Event(Engine::CGameObject* pGameObject)
{
}

void CMonster_Librarian::Damage_Event()
{
    m_pUIBossHP->Set_HPMinus();

    if (m_iHP > 0)
    {
        if(false == m_isGroggy)
            Change_State(STATE_DAMAGE);
    }
    else
        Change_State(STATE_DIE);
}

#include "stdafx.h"
#include "Monster_Librarian.h"
#include "Librarian_Weapon.h"

#include "Librarian_State_Pattern_Energy_Wave.h"
#include "Librarian_State_Pattern_Lightning_Warp.h"
#include "Librarian_State_Pattern_Lunge_Swipe.h"
#include "Librarian_State_Pattern_Homing_Orbs.h"

#include "Librarian_State_Entry.h"
#include "Librarian_State_Idle.h"

#include "Player.h"

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

    /*if (FAILED(Add_Effects()))
        return E_FAIL;*/

    if (FAILED(Add_States()))
        return E_FAIL;

    //m_pModelCom->Set_Animation_Index(ANIM_FLYING_IDLE);
    m_pModelCom->Set_Animation_Transform(m_pTransformCom);
    Set_Animation();

    _float4 vPosition = _float4(0.f, 0.5f, 0.f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    m_iHP = 6;

    return S_OK;
}

HRESULT CMonster_Librarian::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    /*static _uint iIndex = 14;
    if (m_pGameInstance->Get_DIKeyState(DIK_I, KEY_DOWN))
    {
        iIndex++;
        if (iIndex > 35)
            iIndex = 0;
        m_pModelCom->Set_Animation_Index(iIndex);
    }

    m_pModelCom->Play_Animation(fTimeDelta);*/     

    for (auto& PartObject : m_PartObjects)
        PartObject.second->Tick(fTimeDelta);

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

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
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

HRESULT CMonster_Librarian::Add_Effects()
{
    ///* For. Part_Librarian_Effect_Slash */
    //CEffect* pEffect = { nullptr };
    //CEffect_Librarian::EFFECT_LIBRARIAN_DESC tDesc{};

    //tDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
    //tDesc.pSocketBone = m_pModelCom->Get_Bone_Ptr(SWORDBONE);
    //_char szModelTag[MAX_PATH] = "Prototype_Component_Model_Boss_Librarian_Effect_Slash";
    //wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    //tDesc.strModelComTag = wstr;
    ////tDesc.eWeapon = WEAPON_SWORD;

    //pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Part_Librarian_Weapon"), &tDesc));
    //if (nullptr == pEffect)
    //    return E_FAIL;

    //m_PartObjects.emplace(TEXT("Part_Librarian_Effect_Slash"), pEffect);

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

    m_pModelCom->Add_State(STATE_ENTRY, CLibrarian_State_Entry::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_IDLE, CLibrarian_State_Idle::Create(this, pPlayer));

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

    // ROOT
    m_pModelCom->Set_Animation_isRoot(ANIM_DIE, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_FAST_SLAM, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_LIGHTNING_WAIT, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_LIGHTNING_RELEASE, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_LIGHTNING_WINDUP, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_LIGHTNING_LAND, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_FLYING_SWOOP, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_SUMMON_ORB_LOOP, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_PALPATINE, true);
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
}

void CMonster_Librarian::Collision_Event(Engine::CGameObject* pGameObject)
{
}

void CMonster_Librarian::Damage_Event()
{
}

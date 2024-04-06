#include "stdafx.h"
#include "Monster_Librarian.h"
#include "Librarian_Weapon.h"

#include "Librarian_State_Pattern_Lightning_Warp.h"

#include "Player.h"

#define SWORDBONE 79
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

    if (FAILED(Add_States()))
        return E_FAIL;

    _float4 vPosition = _float4(0.f, 1.f,  0.f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    //m_pModelCom->Set_Animation_Index(0);
    m_pModelCom->Set_Animation_Transform(m_pTransformCom);
    Set_Animation();

    m_iHP = 6;

    return S_OK;
}

HRESULT CMonster_Librarian::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

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
    CBounding_SPHERE::BOUNDING_SPHERE_DESC ColliderDesc{};

    ColliderDesc.fRadius = 1.6f;
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
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
    /*m_pModelCom->Add_State(STATE_WALK, CCowBot_State_Walk::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_RUN, CCowBot_State_Run::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_ATTACK, CCowBot_State_Attack::Create(this, pPlayer));*/

    m_pModelCom->Change_State(STATE_PATTERN_LIGHTNING_WARP);
    m_eState = STATE_PATTERN_LIGHTNING_WARP;

    return S_OK;
}

void CMonster_Librarian::Update_State()
{
}

void CMonster_Librarian::Set_Animation()
{
    // LOOP
    m_pModelCom->Set_Animation_isLoop(ANIM_FLYING_IDLE, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_FLYING_IDLE_LEFT, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_FLYING_IDLE_RIGHT, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_FLYING_HAND_UP, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_IDLE_GHOST, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_ENTRY, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_LIGHTNING_LOOP, true);

    // ROOT
    m_pModelCom->Set_Animation_isRoot(ANIM_DIE, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_FAST_SLAM, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_LIGHTNING, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_LIGHTNING_DRAW, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_LIGHTNING_UP, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_LIGHTNING_UP2, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_LIGHTNING_SLAM, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_PALPATINE2, true);
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

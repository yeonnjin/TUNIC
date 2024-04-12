#include "stdafx.h"
#include "Monster_Guard.h"

#include "Guard_Weapon_Shield.h"
#include "Guard_Weapon_Spear.h"

#include "Player.h"

#define SHIELDBONE 27
#define SPEARBONE 32

CMonster_Guard::CMonster_Guard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CMonster_Guard::CMonster_Guard(const CMonster_Guard& rhs)
    : CMonster{ rhs }
{
}

void CMonster_Guard::Change_State(STATE eState)
{
    m_pModelCom->Change_State(eState);
    m_eState = eState;
}

HRESULT CMonster_Guard::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster_Guard::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_PartObjects()))
        return E_FAIL;

    if (FAILED(Add_States()))
        return E_FAIL;

    _float4 vPosition = _float4(-2.f + rand() % 6, 0.5f, rand() % 6 + 5.f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    m_pModelCom->Set_Animation_Index(0);
    m_pModelCom->Set_Animation_Transform(m_pTransformCom);
    Set_Animation();

    m_iHP = 6;

    return S_OK;
}

HRESULT CMonster_Guard::Tick(_float fTimeDelta)
{
    /*if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;*/

    static _uint iIndex = 0;
    if (m_pGameInstance->Get_DIKeyState(DIK_I, KEY_DOWN))
    {
        iIndex++;
        if (iIndex > 9)
            iIndex = 0;
        m_pModelCom->Set_Animation_Index(iIndex);
    }

    m_pModelCom->Play_Animation(fTimeDelta);

    // PartObject
    for (auto& PartObject : m_PartObjects)
        PartObject.second->Tick(fTimeDelta);

    return S_OK;
}

void CMonster_Guard::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    for (auto& PartObject : m_PartObjects)
        PartObject.second->Late_Tick(fTimeDelta);
}

HRESULT CMonster_Guard::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Guard::Add_Components()
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

HRESULT CMonster_Guard::Add_PartObjects()
{
    /* For. Part_Guard_Weapon_Shield */
    CPartObject* pWeaponObject = { nullptr };
    CGuard_Weapon_Shield::GUARD_WEAPON_SHIELD_DESC tDesc{};

    tDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
    tDesc.pSocketBone = m_pModelCom->Get_Bone_Ptr(SHIELDBONE);
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Monster_Guard_Shield";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    tDesc.strModelComTag = wstr;
    tDesc.eWeapon = WEAPON_SHIELD;

    pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Part_Guard_Weapon_Shield"), &tDesc));
    if (nullptr == pWeaponObject)
        return E_FAIL;

    m_PartObjects.emplace(TEXT("Part_Guard_Weapon_Shield"), pWeaponObject);

    /* For. Part_Guard_Weapon_Spear */
    pWeaponObject = { nullptr };
    CGuard_Weapon_Spear::GUARD_WEAPON_SPEAR_DESC tSpearDesc = {};

    tSpearDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
    tSpearDesc.pSocketBone = m_pModelCom->Get_Bone_Ptr(SPEARBONE);
    _char szModelTag1[MAX_PATH] = "Prototype_Component_Model_Monster_Guard_Spear";
    wstring wstr1(&szModelTag1[0], &szModelTag1[MAX_PATH]);
    tSpearDesc.strModelComTag = wstr1;
    tSpearDesc.eWeapon = WEAPON_SPEAR;

    pWeaponObject = dynamic_cast<CPartObject*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Part_Guard_Weapon_Spear"), &tSpearDesc));
    if (nullptr == pWeaponObject)
        return E_FAIL;

    m_PartObjects.emplace(TEXT("Part_Guard_Weapon_Spear"), pWeaponObject);

    return S_OK;
}

HRESULT CMonster_Guard::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Guard::Add_States()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0));
    /*m_pModelCom->Add_State(STATE_IDLE, CCowBot_State_Idle::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_WALK, CCowBot_State_Walk::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_RUN, CCowBot_State_Run::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_DAMAGE, CCowBot_State_Damage::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_DIE, CCowBot_State_Die::Create(this, pPlayer));

    CCowBot_Weapon* pWeapon = dynamic_cast<CCowBot_Weapon*>(m_PartObjects.find(TEXT("Part_CowBot_Weapon_Sword"))->second);
    if (nullptr == pWeapon)
        return E_FAIL;

    m_pModelCom->Add_State(STATE_ATTACK, CCowBot_State_Attack::Create(this, pPlayer, pWeapon));

    m_pModelCom->Change_State(STATE_IDLE);
    m_eState = STATE_IDLE;*/

    return S_OK;
}

void CMonster_Guard::Update_State()
{
}

void CMonster_Guard::Set_Animation()
{
    //// LOOP
//m_pModelCom->Set_Animation_isLoop(ANIM_IDLE, true);
//m_pModelCom->Set_Animation_isLoop(ANIM_RUN, true);
//m_pModelCom->Set_Animation_isLoop(ANIM_WALK, true);
//
//// ROOT
//m_pModelCom->Set_Animation_isRoot(ANIM_RUN, true);

    for (size_t i = 0; i < 9; i++)
        m_pModelCom->Set_Animation_isLoop(i, true);
}

CMonster_Guard* CMonster_Guard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster_Guard* pInstance = new CMonster_Guard(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CMonster_Guard"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster_Guard::Clone(void* pArg)
{
    CMonster_Guard* pInstance = new CMonster_Guard(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CMonster_Guard"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMonster_Guard::Free()
{
    __super::Free();

    for (auto& PartObject : m_PartObjects)
        Safe_Release(PartObject.second);

    m_PartObjects.clear();
}

void CMonster_Guard::Collision_Event(Engine::CGameObject* pGameObject)
{
}

void CMonster_Guard::Damage_Event()
{
}

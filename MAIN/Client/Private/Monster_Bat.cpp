#include "stdafx.h"
#include "Monster_Bat.h"

#include "Bat_State_Idle.h"
#include "Bat_State_Sleep.h"
#include "Bat_State_Attack.h"
#include "Bat_State_Damage.h"

#include "Player.h"

CMonster_Bat::CMonster_Bat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CMonster_Bat::CMonster_Bat(const CMonster_Bat& rhs)
    : CMonster{ rhs }
{
}

void CMonster_Bat::Change_State(STATE eState)
{
    m_pModelCom->Change_State(eState);
    m_eState = eState;
}

HRESULT CMonster_Bat::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster_Bat::Initialize(void* pArg)
{
    MONSTER_BAT_DESC* pDesc = (MONSTER_BAT_DESC*)pArg;
    m_iNavigationIndex = pDesc->iNavigationIndex;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_States()))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPosition);

    m_pModelCom->Set_Animation_Index(ANIM_SLEEPING);
    m_pModelCom->Set_Animation_Transform(m_pTransformCom);

    Set_Animation();

    m_iHP = 5;

    return S_OK;
}

HRESULT CMonster_Bat::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CMonster_Bat::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CMonster_Bat::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Bat::Add_Components()
{
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For. Com_Collider */
    CBounding_SPHERE::BOUNDING_SPHERE_DESC ColliderDesc{};

    ColliderDesc.fRadius = 1.2f;
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    /* For.Com_Navigation */
    CNavigation::NAVIGATION_DESC			NavigationDesc{};
    NavigationDesc.iCurrentIndex = m_iNavigationIndex;
    if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Bat::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Bat::Add_States()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0));
    m_pModelCom->Add_State(STATE_IDLE, CBat_State_Idle::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_SLEEP, CBat_State_Sleep::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_ATTACK, CBat_State_Attack::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_DAMAGE, CBat_State_Damage::Create(this, pPlayer));
    m_pModelCom->Change_State(STATE_IDLE);
    m_eState = STATE_IDLE;

    return S_OK;
}

void CMonster_Bat::Update_State()
{
}

void CMonster_Bat::Set_Animation()
{
    // LOOP
    m_pModelCom->Set_Animation_isLoop(ANIM_IDLE, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_SLEEPING, true);

    // ROOT
    m_pModelCom->Set_Animation_isRoot(ANIM_ATTACK, true);
    m_pModelCom->Set_Animation_isRoot(ANIM_FLINCH, true);
}

CMonster_Bat* CMonster_Bat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster_Bat* pInstance = new CMonster_Bat(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CMonster_Bat"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster_Bat::Clone(void* pArg)
{
    CMonster_Bat* pInstance = new CMonster_Bat(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CMonster_Bat"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMonster_Bat::Free()
{
    __super::Free();
}

void CMonster_Bat::Collision_Event(Engine::CGameObject* pGameObject)
{
    __super::Collision_Event(pGameObject);

    if (OBJ_PLAYER == pGameObject->Get_ObjectType() && STATE_ATTACK == m_eState)
    {
        // 패링 상태 아닐 때 공격
        if (false == dynamic_cast<CPlayer*>(pGameObject)->isParrying())
        {
            dynamic_cast<CPlayer*>(pGameObject)->Set_isDamage(true);
        }
    }
}

void CMonster_Bat::Damage_Event()
{
    if (0 >= m_iHP)
        m_isDead = true;
    else
        Change_State(STATE_DAMAGE);
}

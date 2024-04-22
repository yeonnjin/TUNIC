#include "stdafx.h"
#include "Monster_Blob.h"

#include "Blob_State_Idle.h"
#include "Blob_State_Jump.h"
#include "Blob_State_Attack.h"

#include "Player.h"

CMonster_Blob::CMonster_Blob(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster{ pDevice, pContext }
{
}

CMonster_Blob::CMonster_Blob(const CMonster_Blob& rhs)
    : CMonster{ rhs }
{
}

void CMonster_Blob::Change_State(STATE eState)
{
    m_pModelCom->Change_State(eState);
    m_eState = eState;
}

HRESULT CMonster_Blob::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster_Blob::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_States()))
        return E_FAIL;

    _float4 vPosition = _float4(-76.f + rand() % 3, 2.5f, 76.f + rand() % 3, 1.f);
    //_float4 vPosition = _float4(5.f + rand() % 6, 0.5f, rand() % 6 + 1.f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    m_pModelCom->Set_Animation_Index(ANIM_IDLE);
    m_pModelCom->Set_Animation_Transform(m_pTransformCom);
    m_pModelCom->Set_isUseTransformaion(true);
    Set_Animation();

    m_iHP = 2;

    return S_OK;
}

HRESULT CMonster_Blob::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CMonster_Blob::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CMonster_Blob::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Blob::Add_Components()
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

    ColliderDesc.fRadius = 1.2f;
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    /* For. Com_RigidCollider */
    CBounding_OBB::BOUNDING_OBB_DESC		RigidDesc{};

    // 로컬상의 정보를 셋팅한다.
    RigidDesc.vSize = _float3(1.f, 2.f, 1.f);
    RigidDesc.vCenter = _float3(0.f, RigidDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_RigidCollider"), (CComponent**)&m_pRigidColliderCom, &RigidDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Blob::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Blob::Add_States()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0));
    m_pModelCom->Add_State(STATE_IDLE, CBlob_State_Idle::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_ATTACK, CBlob_State_Attack::Create(this, pPlayer));
    m_pModelCom->Add_State(STATE_JUMP, CBlob_State_Jump::Create(this, pPlayer));
    m_pModelCom->Change_State(STATE_IDLE);
    m_eState = STATE_IDLE;

    return S_OK;
}

void CMonster_Blob::Update_State()
{
}

void CMonster_Blob::Set_Animation()
{
    // LOOP
    m_pModelCom->Set_Animation_isLoop(ANIM_IDLE, true);
    m_pModelCom->Set_Animation_isLoop(ANIM_JUMP, true);

    // ROOT
    //m_pModelCom->Set_Animation_isRoot(ANIM_ATTACK, true);
}

CMonster_Blob* CMonster_Blob::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMonster_Blob* pInstance = new CMonster_Blob(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CMonster_Blob"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster_Blob::Clone(void* pArg)
{
    CMonster_Blob* pInstance = new CMonster_Blob(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CMonster_Blob"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMonster_Blob::Free()
{
    __super::Free();
}

void CMonster_Blob::Collision_Event(Engine::CGameObject* pGameObject)
{
    __super::Collision_Event(pGameObject);

    if (OBJ_PLAYER == pGameObject->Get_ObjectType() && true == m_isAttackFrame)
    {
        // 패링 상태 아닐 때 공격
        if (false == dynamic_cast<CPlayer*>(pGameObject)->isParrying())
        {
            dynamic_cast<CPlayer*>(pGameObject)->Set_isDamage(true);
        }
    }
}

void CMonster_Blob::Damage_Event()
{
    if (0 >= m_iHP)
        m_isDead = true;
}

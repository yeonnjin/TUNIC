#include "stdafx.h"
#include "Object_Ladder.h"

#include "Player.h"
#include "UI_Interactive.h"

CObject_Ladder::CObject_Ladder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteractiveObject{ pDevice, pContext }
{
}

CObject_Ladder::CObject_Ladder(const CObject_Ladder& rhs)
    : CInteractiveObject{ rhs }
{
}

HRESULT CObject_Ladder::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CObject_Ladder::Initialize(void* pArg)
{
    LADDER_DESC* pDesc = (LADDER_DESC*)pArg;
    m_isUpper = pDesc->isUpper;
    m_isRotation = pDesc->isRotation;
    m_isEnd = pDesc->isEnd;
    m_iIndex = pDesc->iIndex;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPosition);

    m_eInteractiveType = INTERACTIVE_LADDER;


    if (true == m_isRotation)
        m_pTransformCom->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(90.f));

    Compute_ColliderMatrix();

    return S_OK;
}

HRESULT CObject_Ladder::Tick(_float fTimeDelta)
{
    if (E_FAIL == __super::Tick(fTimeDelta))
        return E_FAIL;

    // Collider
    m_pColliderCom->Tick(m_ColliderMatrix);

    m_pGameInstance->Add_Group(CCollision_Manager::GROUP_INTERACTIVE, this);

    return S_OK;
}

void CObject_Ladder::Late_Tick(_float fTimeDelta)
{
    m_pUIInteractive->Late_Tick(fTimeDelta);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CObject_Ladder::Render()
{
    return S_OK;
}

HRESULT CObject_Ladder::Add_Components()
{
    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    // 로컬상의 정보를 셋팅한다.
    if(true == m_isUpper)
    {
        ColliderDesc.vSize = _float3(1.5f, 1.f, 0.5f);
        ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * -0.5f + 0.01f, 0.f);
    }
    else
    {
        ColliderDesc.vSize = _float3(1.5f, 1.f, 1.f);
        ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
    }
    

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CObject_Ladder::Bind_ShaderResources()
{
    return S_OK;
}

void CObject_Ladder::Compute_ColliderMatrix()
{
    _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    m_ColliderMatrix = WorldMatrix;
}

CObject_Ladder* CObject_Ladder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CObject_Ladder* pInstance = new CObject_Ladder(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CObject_Ladder"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CObject_Ladder::Clone(void* pArg)
{
    CObject_Ladder* pInstance = new CObject_Ladder(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CObject_Ladder"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CObject_Ladder::Free()
{
    __super::Free();
}

void CObject_Ladder::Collision_Event(Engine::CGameObject* pGameObject)
{
    __super::Collision_Event(pGameObject);

    CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameObject);

    // 플레이어가 사다리를 이용 중이거나 마지막 사다리라면 UI를 띄우지 않음
    if(CPlayer::STATE_CLIMB == pPlayer->Get_State() || true == m_isEnd)
        m_pUIInteractive->Set_Using(false);
}

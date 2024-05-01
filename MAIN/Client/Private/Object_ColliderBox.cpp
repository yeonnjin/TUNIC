#include "stdafx.h"
#include "Object_ColliderBox.h"

CObject_ColliderBox::CObject_ColliderBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteractiveObject{ pDevice, pContext }
{
}

CObject_ColliderBox::CObject_ColliderBox(const CObject_ColliderBox& rhs)
    : CInteractiveObject{ rhs }
{
}

HRESULT CObject_ColliderBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CObject_ColliderBox::Initialize(void* pArg)
{
    COLLIDERBOX_DESC* pDesc = (COLLIDERBOX_DESC*)pArg;
    m_vSize = pDesc->vSize;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPosition);
    if(true == pDesc->isRotation)
        m_pTransformCom->Rotation(_vector{ 0.f, 1.f, 0.f }, XMConvertToRadians(pDesc->fAngle));

    m_eRigid = RIGID_BLOCK;

    return S_OK;
}

HRESULT CObject_ColliderBox::Tick(_float fTimeDelta)
{
    if (E_FAIL == __super::Tick(fTimeDelta))
        return E_FAIL;

    m_pRigidColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    m_pGameInstance->Add_RigidGroup(this);
}

void CObject_ColliderBox::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pRigidColliderCom);
#endif
}

HRESULT CObject_ColliderBox::Render()
{
    return S_OK;
}

HRESULT CObject_ColliderBox::Add_Components()
{
    /* For. Com_RigidCollider */
    CBounding_OBB::BOUNDING_OBB_DESC		RigidDesc{};

    // 로컬상의 정보를 셋팅한다.
    RigidDesc.vSize = m_vSize;
    RigidDesc.vCenter = _float3(0.f, RigidDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_RigidCollider"), (CComponent**)&m_pRigidColliderCom, &RigidDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CObject_ColliderBox::Bind_ShaderResources()
{
    return S_OK;
}

CObject_ColliderBox* CObject_ColliderBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CObject_ColliderBox* pInstance = new CObject_ColliderBox(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CObject_ColliderBox"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CObject_ColliderBox::Clone(void* pArg)
{
    CObject_ColliderBox* pInstance = new CObject_ColliderBox(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CObject_ColliderBox"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CObject_ColliderBox::Free()
{
    __super::Free();

    Safe_Release(m_pRigidColliderCom);
}

void CObject_ColliderBox::Collision_Event(Engine::CGameObject* pGameObject)
{
}

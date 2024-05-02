#include "stdafx.h"
#include "Object_Telescope.h"

CObject_Telescope::CObject_Telescope(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteractiveObject{ pDevice, pContext }
{
}

CObject_Telescope::CObject_Telescope(const CObject_Telescope& rhs)
    : CInteractiveObject{ rhs }
{
}

HRESULT CObject_Telescope::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CObject_Telescope::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    _float4 vPosition = { -5.f, 1.4f, -113.f, 1.f };
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    m_pTransformCom->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f));

    Compute_ColliderMatrix();

    m_eInteractiveType = INTERACTIVE_TELESCOPE;
    m_eRigid = RIGID_BLOCK;

    return S_OK;
}

HRESULT CObject_Telescope::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    m_pColliderCom->Tick(m_ColliderMatrix);

    m_pRigidColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    m_pGameInstance->Add_Group(CCollision_Manager::GROUP_INTERACTIVE, this);

    m_pGameInstance->Add_RigidGroup(this);

    return S_OK;
}

void CObject_Telescope::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pRigidColliderCom);
#endif
}

HRESULT CObject_Telescope::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; ++i)
    {

        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CObject_Telescope::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Object_Telescope";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    if (FAILED(__super::Add_Component(LEVEL_BEACH, wstr,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    // 로컬상의 정보를 셋팅한다.
    ColliderDesc.vSize = _float3(2.f, 1.f, 2.f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    /* For. Com_RigidCollider */
    CBounding_OBB::BOUNDING_OBB_DESC		RigidDesc{};

    // 로컬상의 정보를 셋팅한다.
    RigidDesc.vSize = _float3(2.f, 2.f, 2.f);
    RigidDesc.vCenter = _float3(0.f, RigidDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_RigidCollider"), (CComponent**)&m_pRigidColliderCom, &RigidDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CObject_Telescope::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float fCamFar = m_pGameInstance->Get_Camera_Far();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

void CObject_Telescope::Compute_ColliderMatrix()
{
    _vector vPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vLook = XMVector3Normalize(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
    vPosition += vLook;

    _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    memcpy(&WorldMatrix.r[3], &vPosition, sizeof(_vector));
    m_ColliderMatrix = WorldMatrix;
}

CObject_Telescope* CObject_Telescope::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CObject_Telescope* pInstance = new CObject_Telescope(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CObject_Telescope"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CObject_Telescope::Clone(void* pArg)
{
    CObject_Telescope* pInstance = new CObject_Telescope(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CObject_Telescope"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CObject_Telescope::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pRigidColliderCom);
}

void CObject_Telescope::Collision_Event(Engine::CGameObject* pGameObject)
{
    __super::Collision_Event(pGameObject);
}

void CObject_Telescope::Damage_Event()
{
}

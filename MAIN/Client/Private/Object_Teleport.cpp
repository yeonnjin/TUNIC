#include "stdafx.h"
#include "Object_Teleport.h"

CObject_Teleport::CObject_Teleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteractiveObject{ pDevice, pContext }
{
}

CObject_Teleport::CObject_Teleport(const CObject_Teleport& rhs)
    : CInteractiveObject{ rhs }
{
}

HRESULT CObject_Teleport::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CObject_Teleport::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    _vector vPosition = _vector{ 0.f, 5.f, 0.f, 1.f };
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    m_eRigid = RIGID_NONBLOCK;

    return S_OK;
}

HRESULT CObject_Teleport::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CObject_Teleport::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CObject_Teleport::Render()
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

HRESULT CObject_Teleport::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Object_Teleport";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    if (FAILED(__super::Add_Component(LEVEL_BEACH, wstr,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    // 로컬상의 정보를 셋팅한다.
    ColliderDesc.vSize = _float3(2.f, 2.f, 2.f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CObject_Teleport::Bind_ShaderResources()
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

void CObject_Teleport::Compute_ColliderMatrix()
{
    _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    m_ColliderMatrix = WorldMatrix;
}

CObject_Teleport* CObject_Teleport::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CObject_Teleport* pInstance = new CObject_Teleport(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CObject_Teleport"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CObject_Teleport::Clone(void* pArg)
{
    CObject_Teleport* pInstance = new CObject_Teleport(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CObject_Teleport"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CObject_Teleport::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}

void CObject_Teleport::Collision_Event(Engine::CGameObject* pGameObject)
{
}

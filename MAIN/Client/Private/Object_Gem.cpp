#include "stdafx.h"
#include "Object_Gem.h"

CObject_Gem::CObject_Gem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteractiveObject{ pDevice, pContext }
{
}

CObject_Gem::CObject_Gem(const CObject_Gem& rhs)
    : CInteractiveObject{ rhs }
{
}

HRESULT CObject_Gem::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CObject_Gem::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    _float4 vPosition = { -5.f, 1.4f, -113.f, 1.f };
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    Compute_ColliderMatrix();

    m_eInteractiveType = INTERACTIVE_GEM;
    m_eRigid = RIGID_NONBLOCK;

    return S_OK;
}

HRESULT CObject_Gem::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    m_pColliderCom->Tick(m_ColliderMatrix);

    m_pGameInstance->Add_Group(CCollision_Manager::GROUP_INTERACTIVE, this);

    return S_OK;
}

void CObject_Gem::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CObject_Gem::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CObject_Gem::Add_Components()
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
    ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CObject_Gem::Bind_ShaderResources()
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

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vMtrlDiffuse", &m_vMtrlDiffuse, sizeof(_vector))))
        return E_FAIL;

    return S_OK;
}

void CObject_Gem::Compute_ColliderMatrix()
{
    _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    m_ColliderMatrix = WorldMatrix;
}

CObject_Gem* CObject_Gem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CObject_Gem* pInstance = new CObject_Gem(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CObject_Gem"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CObject_Gem::Clone(void* pArg)
{
    CObject_Gem* pInstance = new CObject_Gem(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CObject_Gem"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CObject_Gem::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}

void CObject_Gem::Collision_Event(Engine::CGameObject* pGameObject)
{
    // UI용
   // __super::Collision_Event(pGameObject);
}

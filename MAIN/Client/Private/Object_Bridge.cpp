#include "stdafx.h"
#include "Object_Bridge.h"

CObject_Bridge::CObject_Bridge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteractiveObject{ pDevice, pContext }
{
}

CObject_Bridge::CObject_Bridge(const CObject_Bridge& rhs)
    : CInteractiveObject{ rhs }
{
}

HRESULT CObject_Bridge::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CObject_Bridge::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    _vector vPosition = _vector{ 0.f, 0.2f, -82.f, 1.f };
    m_pTransformCom->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(-40.f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    m_eRigid = RIGID_NONBLOCK;

    return S_OK;
}

HRESULT CObject_Bridge::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CObject_Bridge::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CObject_Bridge::Render()
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

HRESULT CObject_Bridge::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Object_Bridge";
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

HRESULT CObject_Bridge::Bind_ShaderResources()
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

void CObject_Bridge::Compute_ColliderMatrix()
{
}

CObject_Bridge* CObject_Bridge::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CObject_Bridge* pInstance = new CObject_Bridge(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CObject_Bridge"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CObject_Bridge::Clone(void* pArg)
{
    CObject_Bridge* pInstance = new CObject_Bridge(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CObject_Bridge"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CObject_Bridge::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);
}
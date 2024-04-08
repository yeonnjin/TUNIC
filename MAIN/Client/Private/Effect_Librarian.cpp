#include "stdafx.h"
#include "Effect_Librarian.h"

CEffect_Librarian::CEffect_Librarian(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CEffect{ pDevice, pContext }
{
}

CEffect_Librarian::CEffect_Librarian(const CEffect_Librarian& rhs)
    : CEffect{ rhs }
{
}

HRESULT CEffect_Librarian::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Librarian::Initialize(void* pArg)
{
    EFFECT_LIBRARIAN_DESC* pDesc = (EFFECT_LIBRARIAN_DESC*)pArg;

    m_strModelComTag = pDesc->strModelComTag;
    m_pSocketBone = pDesc->pSocketBone;
    //m_eWeapon = pDesc->eWeapon;

    Safe_AddRef(m_pSocketBone);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Librarian::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    //m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

    return S_OK;
}

void CEffect_Librarian::Late_Tick(_float fTimeDelta)
{
    if (true == m_isRender)
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CEffect_Librarian::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; i++)
    {
        /*if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
            return E_FAIL;*/

        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", i, TEX_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

#ifdef _DEBUG
    m_pColliderCom->Render();
#endif

    return S_OK;
}

HRESULT CEffect_Librarian::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_strModelComTag,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    /* 로컬상의 정보를 셋팅한다. */

    ColliderDesc.vSize = _float3(0.2f, 0.2f, 1.f);
    ColliderDesc.vCenter = _float3(0.f, 0.f, ColliderDesc.vSize.z * -0.5f);

   /* if (CPlayer::WEAPON_STICK == m_eWeapon)
    {
        ColliderDesc.vSize = _float3(0.2f, 0.2f, 1.f);
        ColliderDesc.vCenter = _float3(0.f, 0.f, ColliderDesc.vSize.z * -0.5f);
    }
    else if (CPlayer::WEAPON_SWORD == m_eWeapon)
    {
        ColliderDesc.vSize = _float3(0.4f, 1.8f, 0.4f);
        ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
    }
    else if (CPlayer::WEAPON_SHIELD == m_eWeapon)
    {
        ColliderDesc.vSize = _float3(1.2f, 1.2f, 0.4f);
        ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
    }*/

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Librarian::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

CEffect_Librarian* CEffect_Librarian::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffect_Librarian* pInstance = new CEffect_Librarian(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CEffect_Librarian"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_Librarian::Clone(void* pArg)
{
    CEffect_Librarian* pInstance = new CEffect_Librarian(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CEffect_Librarian"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Librarian::Free()
{
    __super::Free();

    Safe_Release(m_pSocketBone);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}

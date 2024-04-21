#include "stdafx.h"
#include "Item.h"

#include "GameInstance.h"

CItem::CItem()
    : m_pGameInstance(CGameInstance::Get_Instance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CItem::Initialize()
{
    return S_OK;
}

void CItem::Tick(_float fTimeDelta)
{
}

void CItem::Late_Tick(_float fTimeDelta)
{
}

HRESULT CItem::Render()
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

void CItem::Set_TextureInfo()
{
    m_iTextureIndex = (_uint)m_eItem;  
}

HRESULT CItem::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Weapon_Sword";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, wstr,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    // 로컬상의 정보를 셋팅한다.
    ColliderDesc.vSize = _float3(0.4f, 1.8f, 0.4f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CItem::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_ptransform)))
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

CItem* CItem::Create()
{
    CItem* pInstance = new CItem();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CItem"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CItem::Free()
{
    Safe_Release(m_pGameInstance);
}

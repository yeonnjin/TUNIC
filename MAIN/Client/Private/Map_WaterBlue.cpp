#include "stdafx.h"
#include "Map_WaterBlue.h"

CMap_WaterBlue::CMap_WaterBlue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBlendObject{ pDevice, pContext }
{
}

CMap_WaterBlue::CMap_WaterBlue(const CMap_WaterBlue& rhs)
    : CBlendObject{ rhs }
{
}

HRESULT CMap_WaterBlue::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMap_WaterBlue::Initialize(void* pArg)
{
    CGameObject::GAMEOBJECT_DESC tDesc{};
    tDesc.fRotationPerSec = XMConvertToRadians(90.f);
    tDesc.fSpeedPerSec = 3.f;

    if (FAILED(__super::Initialize(&tDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    _vector vPosition = { 0.f, -2.5f, 0.f, 1.f };
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
    m_pTransformCom->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f));
    m_pTransformCom->Set_Scaled(100.f, 100.f, 100.f);

    return S_OK;
}

HRESULT CMap_WaterBlue::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    return S_OK;
}

void CMap_WaterBlue::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CMap_WaterBlue::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(3)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CMap_WaterBlue::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMeshMap"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Map_Beach_WaterBlue";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    if (FAILED(__super::Add_Component(LEVEL_BEACH, wstr,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMap_WaterBlue::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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

CMap_WaterBlue* CMap_WaterBlue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMap_WaterBlue* pInstance = new CMap_WaterBlue(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CMap_WaterBlue"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMap_WaterBlue::Clone(void* pArg)
{
    CMap_WaterBlue* pInstance = new CMap_WaterBlue(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Create : CMap_WaterBlue"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMap_WaterBlue::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}

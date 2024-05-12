#include "stdafx.h"
#include "Map_WaterDepth.h"

CMap_WaterDepth::CMap_WaterDepth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBlendObject{ pDevice, pContext }
{
}

CMap_WaterDepth::CMap_WaterDepth(const CMap_WaterDepth& rhs)
    : CBlendObject{ rhs }
{
}

HRESULT CMap_WaterDepth::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMap_WaterDepth::Initialize(void* pArg)
{
    CGameObject::GAMEOBJECT_DESC tDesc{};
    tDesc.fRotationPerSec = XMConvertToRadians(90.f);
    tDesc.fSpeedPerSec = 3.f;

    if (FAILED(__super::Initialize(&tDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    //_vector vPosition = { 0.f, 0.01f, 0.f, 1.f };
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
    m_pTransformCom->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f));
    m_pTransformCom->Set_Scaled(100.f, 100.f, 80.f);

    return S_OK;
}

HRESULT CMap_WaterDepth::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    return S_OK;
}

void CMap_WaterDepth::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CMap_WaterDepth::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; ++i)
    {

        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(2)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CMap_WaterDepth::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMeshMap"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Map_Beach_WaterDepth";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    if (FAILED(__super::Add_Component(LEVEL_BEACH, wstr,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMap_WaterDepth::Bind_ShaderResources()
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

    return S_OK;
}

CMap_WaterDepth* CMap_WaterDepth::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMap_WaterDepth* pInstance = new CMap_WaterDepth(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CMap_WaterDepth"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMap_WaterDepth::Clone(void* pArg)
{
    CMap_WaterDepth* pInstance = new CMap_WaterDepth(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Create : CMap_WaterDepth"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMap_WaterDepth::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}

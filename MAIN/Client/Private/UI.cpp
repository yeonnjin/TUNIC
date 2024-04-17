#include "stdafx.h"
#include "UI.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CUI::CUI(const CUI& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
    GAMEOBJECT_DESC		GameObjectDesc{};

    GameObjectDesc.fSpeedPerSec = 3.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    /*if (nullptr != pArg)
    {
        MONSTER_DESC* pDesc = (MONSTER_DESC*)pArg;
        m_strModelComTag = pDesc->strModelComTag;
    }*/

    //if (FAILED(Add_Components()))
    //    return E_FAIL;


    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

    _uint			iNumViewports = { 1 };
    D3D11_VIEWPORT	ViewportDesc = {};
    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

    m_eType = OBJ_UI;

    return S_OK;
}

HRESULT CUI::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;
}

void CUI::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI::Render()
{
    for (size_t i = 0; i < m_UIDescs.size(); i++)
    {
        if (true == m_UIDescs[i].isRender)
        {
            m_iBindTextureIndex = m_UIDescs[i].iBindTextureIndex;
            m_iBindTransformIndex = m_UIDescs[i].iBindTransformIndex;
            m_eUIType = m_UIDescs[i].eShader;

            if (FAILED(Bind_ShaderResources()))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin((_uint)m_eUIType)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Bind_Buffers()))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CUI::Add_Components()
{
    /* For. Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    // View
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    // Proj
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    return S_OK;
}

void CUI::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);

    for (auto& pTransform : m_pUITransformComs)
        Safe_Release(pTransform);
}

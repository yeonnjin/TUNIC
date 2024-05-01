#include "stdafx.h"
#include "UI_Item.h"

CUI_Item::CUI_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

void CUI_Item::Set_TextureIndex(_uint iTextureIndex)
{
    m_UIDescs[0].iBindTextureIndex = iTextureIndex;
}

void CUI_Item::Set_Position(_float2 vPosition)
{
    m_vPosition = vPosition;
    m_pUITransformComs[0]->Set_State(CTransform::STATE_POSITION, _vector{g_iWinSizeX * -0.5f + m_vPosition.x, g_iWinSizeY * 0.5f - m_vPosition.y, 0.5f, 1.f});
}

HRESULT CUI_Item::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Item::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (nullptr != pArg)
    {
        UIITEM_DESC* pDesc = (UIITEM_DESC*)pArg;
        m_iBindTextureIndex = pDesc->iTextureIndex;
        m_vPosition = pDesc->vPosition;
        m_fSize = pDesc->fSize;
    }

    if (FAILED(Set_UIInfo()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Item::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CUI_Item::Late_Tick(_float fTimeDelta)
{
    if(true == m_isUsing)
        __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Item::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Item::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Item"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Item::Bind_ShaderResources()
{
    // MATRIX
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    // World
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pUITransformComs[m_iBindTransformIndex]->Get_WorldFloat4x4_Ptr())))
        return E_FAIL;

    // TEXTURE
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iBindTextureIndex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Item::Set_UIInfo()
{
    // 0 - ITEM
    CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(m_fSize, m_fSize, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + m_vPosition.x, g_iWinSizeY * 0.5f - m_vPosition.y, 0.5f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    UI_DESC tDesc = {};
    tDesc.iBindTextureIndex = m_iBindTextureIndex;
    tDesc.iBindTransformIndex = 0;
    tDesc.eShader = (CUI::UI_SHADER)0;
    m_UIDescs.emplace_back(tDesc);

    return S_OK;
}

CUI_Item* CUI_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Item* pInstance = new CUI_Item(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CUI_Item"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Item::Clone(void* pArg)
{
    CUI_Item* pInstance = new CUI_Item(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CUI_Item"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Item::Free()
{
    __super::Free();
}
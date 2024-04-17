#include "stdafx.h"
#include "UI_Slot.h"

CUI_Slot::CUI_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

HRESULT CUI_Slot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Slot::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_UIInfo()))
        return E_FAIL;

    m_pTransformCom->Set_Scaled(128.f, 128.f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 0.f, 0.f, 0.f, 1.f });

    return S_OK;
}

HRESULT CUI_Slot::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CUI_Slot::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Slot::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Slot::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Inventory"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Slot::Bind_ShaderResources()
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

HRESULT CUI_Slot::Set_UIInfo()
{
    // 0 - BG
    CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    _float fSizeX, fSizeY;

    fSizeX = 211.f;
    fSizeY = 106.f;
    pTransform->Set_Scaled(fSizeX, fSizeY, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + 1173.f, g_iWinSizeY * 0.5f - 97.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    UI_DESC tDesc = {};
    tDesc.iBindTextureIndex = 3;
    tDesc.iBindTransformIndex = 0;
    tDesc.eShader = UI_STAT;
    m_UIDescs.emplace_back(tDesc);

    return S_OK;
}

CUI_Slot* CUI_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Slot* pInstance = new CUI_Slot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CUI_Slot"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Slot::Clone(void* pArg)
{
    CUI_Slot* pInstance = new CUI_Slot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CUI_Slot"));

        Safe_Release(pInstance);
    }

    return pInstance;
}


void CUI_Slot::Free()
{
    __super::Free();
}
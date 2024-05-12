#include "stdafx.h"
#include "UI_Menu.h"

CUI_Menu::CUI_Menu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

void CUI_Menu::Set_Using(_bool isUsing)
{
    m_isUsing = isUsing;

    if(false == m_isUsing)
        m_UIDescs[0].isRender = false;
}

void CUI_Menu::Set_Index(_uint iIndex)
{
    m_UIDescs[0].iBindTextureIndex = 8;
}

HRESULT CUI_Menu::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Menu::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_UIInfo()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Menu::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CUI_Menu::Late_Tick(_float fTimeDelta)
{
    if (true == m_isUsing)
    {
        __super::Late_Tick(fTimeDelta);
    }
}

HRESULT CUI_Menu::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Menu::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Loading"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Menu::Bind_ShaderResources()
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

HRESULT CUI_Menu::Set_UIInfo()
{
    // 0 - BG
    CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(1280.f, 720.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ 0.f, 0.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    UI_DESC tDesc = {};
    tDesc.iBindTextureIndex = 0;
    tDesc.iBindTransformIndex = 0;
    tDesc.eShader = (CUI::UI_SHADER)0;
    tDesc.isRender = true;
    m_UIDescs.emplace_back(tDesc);

    return S_OK;
}

CUI_Menu* CUI_Menu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Menu* pInstance = new CUI_Menu(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CUI_Menu"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Menu::Clone(void* pArg)
{
    CUI_Menu* pInstance = new CUI_Menu(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CUI_Menu"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Menu::Free()
{
    __super::Free();
}
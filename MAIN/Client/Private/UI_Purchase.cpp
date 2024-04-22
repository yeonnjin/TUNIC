#include "stdafx.h"
#include "UI_Purchase.h"

CUI_Purchase::CUI_Purchase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

HRESULT CUI_Purchase::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Purchase::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_UIInfo()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Purchase::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    if (true == m_isUsing)
    {
        Set_SelectRenderOn();

        if (true == m_isOK && m_pGameInstance->Get_DIKeyState(DIK_RIGHT, KEY_DOWN))
            m_isOK = false;
        else if(false == m_isOK && m_pGameInstance->Get_DIKeyState(DIK_LEFT, KEY_DOWN))
            m_isOK = true;

        if (true == m_isOK)
        {
            m_pUITransformComs[3]->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + m_vSelectPosition[0].x, g_iWinSizeY * 0.5f - m_vSelectPosition[0].y, 0.f, 1.f });
        }
        else
        {
            m_pUITransformComs[3]->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + m_vSelectPosition[1].x, g_iWinSizeY * 0.5f - m_vSelectPosition[1].y, 0.f, 1.f });
        }
    }
    else
        Set_RenderOff();

    return S_OK;
}

void CUI_Purchase::Late_Tick(_float fTimeDelta)
{
    if(true == m_isUsing)
        __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Purchase::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Purchase::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Shop"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Purchase::Bind_ShaderResources()
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

HRESULT CUI_Purchase::Set_UIInfo()
{
    // 0 - Default_100
    CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(834.f, 417.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 640.f, g_iWinSizeY * 0.5f - 335.f, 0.9f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    UI_DESC tDesc = {};
    tDesc.iBindTextureIndex = 0;
    tDesc.iBindTransformIndex = 0;
    m_UIDescs.emplace_back(tDesc);

    // 1 - Default_1000
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(834.f, 417.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 640.f, g_iWinSizeY * 0.5f - 335.f, 0.9f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 1;
    tDesc.iBindTransformIndex = 1;
    m_UIDescs.emplace_back(tDesc);

    // 2 - Default_2000
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(834.f, 417.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 640.f, g_iWinSizeY * 0.5f - 335.f, 0.9f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 2;
    tDesc.iBindTransformIndex = 2;
    m_UIDescs.emplace_back(tDesc);

    // 3 - Select Box
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(416.f, 104.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 450.f, g_iWinSizeY * 0.5f - 389.f, 0.9f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 3;
    tDesc.iBindTransformIndex = 3;
    m_UIDescs.emplace_back(tDesc);

    Set_RenderOff();

    m_vSelectPosition[0] = { 450.f, 389.f };
    m_vSelectPosition[1] = { 830.f, 389.f };

    return S_OK;
}

void CUI_Purchase::Set_RenderOff()
{
    for (auto& tUI : m_UIDescs)
        tUI.isRender = false;

    m_isOK = true;
}

void CUI_Purchase::Set_SelectRenderOn()
{
    if (CItem::ITEM_HP == m_eSelectItem || CItem::ITEM_MP == m_eSelectItem)
        m_UIDescs[0].isRender = true;
    else if(CItem::ITEM_DASH == m_eSelectItem)
        m_UIDescs[2].isRender = true;
    else if (CItem::ITEM_POTION == m_eSelectItem)
        m_UIDescs[1].isRender = true;

    // SELECT BOX
    m_UIDescs[3].isRender = true;
}

CUI_Purchase* CUI_Purchase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Purchase* pInstance = new CUI_Purchase(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CUI_Purchase"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Purchase::Clone(void* pArg)
{
    CUI_Purchase* pInstance = new CUI_Purchase(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CUI_Purchase"));

        Safe_Release(pInstance);
    }

    return pInstance;
}


void CUI_Purchase::Free()
{
    __super::Free();
}
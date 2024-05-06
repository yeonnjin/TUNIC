#include "stdafx.h"
#include "UI_Obtain.h"

#include "UI_Item.h"

CUI_Obtain::CUI_Obtain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

void CUI_Obtain::Set_Using(_bool isUsing, CItem::ITEM eItem)
{  
    m_isUsing = isUsing;
    m_eSelectItem = eItem;
    m_pUIItem->Set_TextureIndex((_uint)eItem);   
}

HRESULT CUI_Obtain::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Obtain::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_UIInfo()))
        return E_FAIL;

    CUI_Item::UIITEM_DESC tDesc{};
    tDesc.iTextureIndex = 0;
    tDesc.vPosition = {640.f, 242.f};
    tDesc.fSize = 164.f;
    tDesc.isObtain = true;
    m_pUIItem = dynamic_cast<CUI_Item*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_UI_Item"), &tDesc));
    if (nullptr == m_pUIItem)
        return E_FAIL;

    m_pUIItem->Set_isUsing(false);

    return S_OK;
}

HRESULT CUI_Obtain::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    if (true == m_isUsing)
    {
        m_pUIItem->Set_isUsing(true);
        m_pUIItem->Tick(fTimeDelta);

        m_fAccShowTime += fTimeDelta;
        if (m_fAccShowTime > m_fShowTime)
        {
            m_isUsing = false;
            m_pUIItem->Set_isUsing(false);
            m_fAccShowTime = 0.f;
        }
    }

    return S_OK;
}

void CUI_Obtain::Late_Tick(_float fTimeDelta)
{
    if (true == m_isUsing)
    {
        __super::Late_Tick(fTimeDelta);
        m_pUIItem->Late_Tick(fTimeDelta);
    }
}

HRESULT CUI_Obtain::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Obtain::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Inventory"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Obtain::Bind_ShaderResources()
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

HRESULT CUI_Obtain::Set_UIInfo()
{
    // 0 - Obtain UI
    CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    _float fSizeX, fSizeY;

    fSizeX = 555.f;
    fSizeY = 555.f;
    pTransform->Set_Scaled(fSizeX, fSizeY, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + 640.f, g_iWinSizeY * 0.5f - 298.f, 0.7f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    UI_DESC tDesc = {};
    tDesc.iBindTextureIndex = 5;
    tDesc.iBindTransformIndex = 0;
    tDesc.eShader = UI_STAT;
    m_UIDescs.emplace_back(tDesc);

    return S_OK;
}

CUI_Obtain* CUI_Obtain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Obtain* pInstance = new CUI_Obtain(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CUI_Obtain"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Obtain::Clone(void* pArg)
{
    CUI_Obtain* pInstance = new CUI_Obtain(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CUI_Obtain"));

        Safe_Release(pInstance);
    }

    return pInstance;
}


void CUI_Obtain::Free()
{
    __super::Free();

    Safe_Release(m_pUIItem);
}
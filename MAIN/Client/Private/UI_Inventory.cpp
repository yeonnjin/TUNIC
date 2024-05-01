#include "stdafx.h"
#include "UI_Inventory.h"

CUI_Inventory::CUI_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

HRESULT CUI_Inventory::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Inventory::Initialize(void* pArg)
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

void CUI_Inventory::Set_Select(_uint iRow, _uint iColumn)
{
    m_pUITransformComs[2]->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + m_vPositions[iRow][iColumn].x, g_iWinSizeY * 0.5f - m_vPositions[iRow][iColumn].y, 0.f, 1.f });
}

HRESULT CUI_Inventory::Tick(_float fTimeDelta)
{
    if(true == m_isUsing)
    {
        if (FAILED(__super::Tick(fTimeDelta)))
            return E_FAIL;

       

        /*static _uint i = 0;
        static _uint j = 0;

        m_pUITransformComs[2]->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + m_vPositions[m_iSelectRow][m_iSelectColumn].x, g_iWinSizeY * 0.5f - m_vPositions[m_iSelectRow][m_iSelectColumn].y, 0.f, 1.f });

        if (true == m_pGameInstance->Get_DIKeyState(DIK_Y, KEY_DOWN))
        {
            ++j;
            if (j == 4)
            {
                ++i;
                j = 0;

                if (i == CItem::TYPE_END)
                {
                    i = 0;
                }
            }
        }*/
    }

    return S_OK;
}

void CUI_Inventory::Late_Tick(_float fTimeDelta)
{
    if(true == m_isUsing)
    {
        __super::Late_Tick(fTimeDelta);
    }
}

HRESULT CUI_Inventory::Render()
{
    if (true == m_isUsing)
    {
        if (FAILED(__super::Render()))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CUI_Inventory::Add_Components()
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

HRESULT CUI_Inventory::Bind_ShaderResources()
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

    if(FAILED(m_pShaderCom->Bind_RawValue("g_fSizeX", &m_fBGSizeX, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Inventory::Set_UIInfo()
{
    // 0 - BG
    CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    _float fSizeX, fSizeY;

    m_fBGSizeX = 550.f;
    m_fBGSizeY = 720.f;
    pTransform->Set_Scaled(m_fBGSizeX, m_fBGSizeY, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + m_fBGSizeX * 0.5f, 0.f, 0.8f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    UI_DESC tDesc = {};
    tDesc.iBindTextureIndex = 0;
    tDesc.iBindTransformIndex = 0;
    tDesc.eShader = UI_INVENTORYBG;
    m_UIDescs.emplace_back(tDesc);

    // 1 - INVENTORY
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    fSizeX = 720.f;
    fSizeY = 720.f;
    pTransform->Set_Scaled(fSizeX, fSizeY, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + fSizeX * 0.5f, 0.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 1;
    tDesc.iBindTransformIndex = 1;
    tDesc.eShader = UI_STAT;
    m_UIDescs.emplace_back(tDesc);

    // 2 - SELECT
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    fSizeX = 70.f;
    fSizeY = 70.f;
    pTransform->Set_Scaled(fSizeX, fSizeY, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + 128.f, g_iWinSizeY * 0.5f - 395.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 2;
    tDesc.iBindTransformIndex = 2;
    tDesc.eShader = UI_STAT;
    m_UIDescs.emplace_back(tDesc);


    // POSITION
    _uint iMaxItem = 4;
    _float fMinSizeX = 128.f;
    _float fDistanceX = 98.f;
    _float fUtileY = 262.f;
    _float fUseY = 525.f;
    _float fDistanceY = 147.f;

    for (size_t i = 0; i < CItem::TYPE_END; i++)
    {
        for (size_t j = 0; j < iMaxItem; j++)
        {
            if (CItem::TYPE_USE == i || CItem::TYPE_WEAPON == i)
            {
                m_vPositions[i][j] = { fMinSizeX + j * fDistanceX, fUseY + ( i - 1 ) * fDistanceY };
            }
            else
                m_vPositions[i][j] = { fMinSizeX + j * fDistanceX, fUtileY + i * fDistanceY };
        }
    }

    return S_OK;
}

CUI_Inventory* CUI_Inventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Inventory* pInstance = new CUI_Inventory(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CUI_Inventory"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Inventory::Clone(void* pArg)
{
    CUI_Inventory* pInstance = new CUI_Inventory(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CUI_Inventory"));

        Safe_Release(pInstance);
    }

    return pInstance;
}


void CUI_Inventory::Free()
{
    __super::Free();
}
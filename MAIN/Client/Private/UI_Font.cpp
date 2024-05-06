#include "stdafx.h"
#include "UI_Font.h"

#include <tchar.h>

CUI_Font::CUI_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

HRESULT CUI_Font::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Font::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    _float fDistanceX = 90.f;
    UI_FONT_DESC* pDesc = (UI_FONT_DESC*)pArg;
    m_vPosition = pDesc->vPosition;
    m_vPosition.x -= fDistanceX;
    m_pCount = pDesc->pCount;

    return S_OK;
}

HRESULT CUI_Font::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CUI_Font::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FONT, this);
}

HRESULT CUI_Font::Render()
{
    _stprintf_s(m_szFont, _countof(m_szFont), _T("%d"), *m_pCount);

    m_pGameInstance->Render_Font(TEXT("Font_Odin_35"), m_szFont, _float2(m_vPosition.x, m_vPosition.y), XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f);
    m_pGameInstance->Render_Font(TEXT("Font_Odin_25"), m_szFont, _float2(m_vPosition.x + 6.f, m_vPosition.y + 6.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f);

    return S_OK;
}

CUI_Font* CUI_Font::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Font* pInstance = new CUI_Font(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CUI_Font"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Font::Clone(void* pArg)
{
    CUI_Font* pInstance = new CUI_Font(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CUI_Font"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Font::Free()
{
    __super::Free();
}
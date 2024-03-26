#include "Font_Manager.h"

#include "MyFont.h"

CFont_Manager::CFont_Manager()
{
}

HRESULT CFont_Manager::Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontTag, const wstring& strFontFilePath)
{
    if (nullptr != Find_Font(strFontTag))
        return E_FAIL;

    CMyFont* pFont = CMyFont::Create(pDevice, pContext, strFontFilePath);
    if (nullptr == pFont)
        return E_FAIL;

    m_Fonts.emplace(strFontTag, pFont);

    return S_OK;
}

HRESULT CFont_Manager::Render_Font(const wstring& strFontTag, const wstring& strOutputText, const _float2& vPosition, _fvector vColor, _float fRadian)
{
    CMyFont* pFont = Find_Font(strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    return pFont->Render(strOutputText, vPosition, vColor, fRadian);
}

CMyFont* CFont_Manager::Find_Font(const wstring& strFontTag)
{
    auto iter = m_Fonts.find(strFontTag);
    if (iter == m_Fonts.end())
        return nullptr;

    return iter->second;
}

CFont_Manager* CFont_Manager::Create()
{
    return new CFont_Manager();
}

void CFont_Manager::Free()
{
    for (auto& Pair : m_Fonts)
        Safe_Release(Pair.second);

    m_Fonts.clear();
}

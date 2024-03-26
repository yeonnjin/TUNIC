#include "MyFont.h"

CMyFont::CMyFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext {pContext}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CMyFont::Initialize(const wstring& strFontFilePath)
{
    m_pFont = new SpriteFont(m_pDevice, strFontFilePath.c_str());
    m_pBatch = new SpriteBatch(m_pContext);

    return S_OK;
}

HRESULT CMyFont::Render(const wstring& strOutputText, const _float2& vPosition, _fvector vColor, _float fRadian)
{
    m_pBatch->Begin();

    m_pFont->DrawString(m_pBatch, strOutputText.c_str(), vPosition, vColor, fRadian);

    m_pBatch->End();

    return S_OK;
}

CMyFont* CMyFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontFilePath)
{
    CMyFont* pInstance = new CMyFont(pDevice, pContext);

    if (FAILED(pInstance->Initialize(strFontFilePath)))
    {
        MSG_BOX(TEXT("Failed to Create : CMyFont"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMyFont::Free()
{
    Safe_Delete_Array(m_pBatch);
    Safe_Delete_Array(m_pFont);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}

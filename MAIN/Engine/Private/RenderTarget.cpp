#include "RenderTarget.h"

CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    D3D11_TEXTURE2D_DESC    TextureDesc{};
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    TextureDesc.Width = iSizeX;
    TextureDesc.Height = iSizeY;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = ePixelFormat;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
        return E_FAIL;

    if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
        return E_FAIL;

    m_vClearColor = vClearColor;

    return S_OK;
}

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

    if (FAILED(pInstance->Initialize(iSizeY, iSizeY, ePixelFormat, vClearColor)))
    {
        MSG_BOX(TEXT("Failed To Create : CRenderTarget"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderTarget::Free()
{
    SaveDDSTextureToFile(m_pContext, m_pTexture2D, TEXT("../Bin/Diffuse.dds"));

    Safe_Release(m_pRTV);
    Safe_Release(m_pTexture2D);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}

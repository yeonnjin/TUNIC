#include "stdafx.h"
#include "Sprite.h"

CSprite::CSprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

CSprite::CSprite(const CSprite& rhs)
    : CUI{ rhs }
{
}

HRESULT CSprite::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSprite::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CSprite::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CSprite::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite::Bind_ShaderResources()
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

void CSprite::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}

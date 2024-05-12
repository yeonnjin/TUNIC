#include "stdafx.h"
#include "Sprite_Attack_Stick.h"

CSprite_Attack_Stick::CSprite_Attack_Stick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CSprite_Attack{ pDevice, pContext }
{
}

HRESULT CSprite_Attack_Stick::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSprite_Attack_Stick::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_UIInfo()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite_Attack_Stick::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CSprite_Attack_Stick::Late_Tick(_float fTimeDelta)
{
    if (true == m_isUsing)
        __super::Late_Tick(fTimeDelta);
}

HRESULT CSprite_Attack_Stick::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite_Attack_Stick::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sprite_Attack_Stick"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite_Attack_Stick::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite_Attack_Stick::Set_UIInfo()
{
    // 0 - ATTACK
    CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(m_fSize, m_fSize, 1.f);
    m_pUITransformComs.emplace_back(pTransform);

    UI_DESC tDesc = {};
    tDesc.iBindTextureIndex = 0;
    tDesc.iBindTransformIndex = 0;
    tDesc.eShader = UI_STAT;
    tDesc.isRender = false;
    m_UIDescs.emplace_back(tDesc);

    return S_OK;
}

CSprite_Attack_Stick* CSprite_Attack_Stick::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSprite_Attack_Stick* pInstance = new CSprite_Attack_Stick(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CSprite_Attack_Stick"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSprite_Attack_Stick::Clone(void* pArg)
{
    CSprite_Attack_Stick* pInstance = new CSprite_Attack_Stick(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CSprite_Attack_Stick"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSprite_Attack_Stick::Free()
{
    __super::Free();
}

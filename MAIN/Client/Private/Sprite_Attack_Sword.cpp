#include "stdafx.h"
#include "Sprite_Attack_Sword.h"

CSprite_Attack_Sword::CSprite_Attack_Sword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CSprite_Attack{ pDevice, pContext }
{
}

HRESULT CSprite_Attack_Sword::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSprite_Attack_Sword::Initialize(void* pArg)
{
    m_iMaxIndex = 5;

    m_fSize = 150.f;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_UIInfo()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite_Attack_Sword::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CSprite_Attack_Sword::Late_Tick(_float fTimeDelta)
{
    if (true == m_isUsing)
    {
        __super::Late_Tick(fTimeDelta);
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
    }
}

HRESULT CSprite_Attack_Sword::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite_Attack_Sword::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sprite_Attack_Sword"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite_Attack_Sword::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite_Attack_Sword::Set_UIInfo()
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

CSprite_Attack_Sword* CSprite_Attack_Sword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSprite_Attack_Sword* pInstance = new CSprite_Attack_Sword(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CSprite_Attack_Sword"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSprite_Attack_Sword::Clone(void* pArg)
{
    CSprite_Attack_Sword* pInstance = new CSprite_Attack_Sword(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CSprite_Attack_Sword"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSprite_Attack_Sword::Free()
{
    __super::Free();
}

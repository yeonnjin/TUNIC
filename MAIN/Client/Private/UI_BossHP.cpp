#include "stdafx.h"
#include "UI_BossHP.h"

CUI_BossHP::CUI_BossHP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

HRESULT CUI_BossHP::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_BossHP::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_UIInfo()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_BossHP::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    if (false == m_isUsing)
        return S_OK;

    // 체력이 0일 때
    if (0 == m_fHP)
    {
        m_UIDescs[1].isRender = false;
        m_UIDescs[2].isRender = false;
        m_UIDescs[3].isRender = false;

        return S_OK;
    }

    // Gauge
    if (m_fHP != m_fRenderHP)
    {
        m_fRenderHP -= 0.1f;

        if (m_fRenderHP < m_fHP)
            m_fRenderHP = m_fHP;
    }

    _float3     vHPScale = m_pUITransformComs[2]->Get_Scaled();
    _float      fHPRatio = m_fRenderHP / m_fMaxHP;
    vHPScale.x = m_fHPSize * fHPRatio;

    m_pUITransformComs[2]->Set_Scaled(vHPScale.x, vHPScale.y, vHPScale.z);
    _float4 vHP_MID_Position = m_pUITransformComs[2]->Get_State_Float4(CTransform::STATE_POSITION);
    vHP_MID_Position.x = g_iWinSizeX * -0.5 + 920.5f + m_fHPSize * 0.5f * (1 - fHPRatio);
    m_pUITransformComs[2]->Set_State(CTransform::STATE_POSITION, vHP_MID_Position);

    _float4 vHP_LEFT_Position = m_pUITransformComs[1]->Get_State_Float4(CTransform::STATE_POSITION);
    vHP_LEFT_Position.x = g_iWinSizeX * -0.5 + 597.f + m_fHPSize * (1 - fHPRatio);
    m_pUITransformComs[1]->Set_State(CTransform::STATE_POSITION, vHP_LEFT_Position);

    return S_OK;
}

void CUI_BossHP::Late_Tick(_float fTimeDelta)
{
    if(true == m_isUsing)
        __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_BossHP::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_BossHP::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossHP"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_BossHP::Bind_ShaderResources()
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

HRESULT CUI_BossHP::Set_UIInfo()
{
    // 0 - BG
    CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(720.f, 256.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 920.f, g_iWinSizeY * 0.5f - 582.f, 0.9f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    UI_DESC tDesc = {};
    tDesc.iBindTextureIndex = 0;
    tDesc.iBindTransformIndex = 0;
    tDesc.eShader = UI_STAT;
    m_UIDescs.emplace_back(tDesc);

    // 1 - Left
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(64.f, 64.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 597.f, g_iWinSizeY * 0.5f - 687.f, 0.8f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 1;
    tDesc.iBindTransformIndex = 1;
    tDesc.eShader = UI_STAT;
    m_UIDescs.emplace_back(tDesc);

    // 2 - Gauge
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(m_fHPSize, 64.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 920.5f, g_iWinSizeY * 0.5f - 687.f, 0.8f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 2;
    tDesc.iBindTransformIndex = 2;
    tDesc.eShader = UI_STAT;
    m_UIDescs.emplace_back(tDesc);

    // 3 - Right
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(64.f, 64.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 1244.f, g_iWinSizeY * 0.5f - 687.f, 0.8f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 3;
    tDesc.iBindTransformIndex = 3;
    tDesc.eShader = UI_STAT;
    m_UIDescs.emplace_back(tDesc);

    // 4 - Slide
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(720.f, 256.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 921.f, g_iWinSizeY * 0.5f - 583.f, 0.7f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 4;
    tDesc.iBindTransformIndex = 4;
    tDesc.eShader = UI_STAT;
    m_UIDescs.emplace_back(tDesc);

    return S_OK;
}

CUI_BossHP* CUI_BossHP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_BossHP* pInstance = new CUI_BossHP(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CUI_BossHP"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_BossHP::Clone(void* pArg)
{
    CUI_BossHP* pInstance = new CUI_BossHP(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CUI_BossHP"));

        Safe_Release(pInstance);
    }

    return pInstance;
}


void CUI_BossHP::Free()
{
    __super::Free();
}

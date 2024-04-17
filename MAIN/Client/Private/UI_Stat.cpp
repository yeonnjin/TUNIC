#include "stdafx.h"
#include "UI_Stat.h"

CUI_Stat::CUI_Stat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

HRESULT CUI_Stat::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Stat::Initialize(void* pArg)
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

HRESULT CUI_Stat::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    // HP
    _float3     vHPScale = m_pUITransformComs[4]->Get_Scaled();
    _float      fHPRatio = m_iHP / (_float)m_iMaxHP;
    vHPScale.y = m_fHPSize * fHPRatio;

    if (vHPScale.y <= 0)
        vHPScale.y = 0.01f;

    m_pUITransformComs[4]->Set_Scaled(vHPScale.x, vHPScale.y, vHPScale.z);
    _float4 vHP_MID_Position = m_pUITransformComs[4]->Get_State_Float4(CTransform::STATE_POSITION);
    vHP_MID_Position.y = g_iWinSizeY * -0.5f + 95.f - m_fHPSize * 0.5f * (1 - fHPRatio);
    m_pUITransformComs[4]->Set_State(CTransform::STATE_POSITION, vHP_MID_Position);

    _float4 vHP_TOP_Position = m_pUITransformComs[5]->Get_State_Float4(CTransform::STATE_POSITION);
    vHP_TOP_Position.y = g_iWinSizeY * -0.5f + 146.f - m_fHPSize * (1 - fHPRatio);
    m_pUITransformComs[5]->Set_State(CTransform::STATE_POSITION, vHP_TOP_Position);


    // SP
    _float3     vSPScale = m_pUITransformComs[7]->Get_Scaled();
    _float      fSPRatio = m_fSP / m_fMaxSP;
    vSPScale.y = m_fSPSize * fSPRatio;

    if (vSPScale.y <= 0)
        vSPScale.y = 0.01f;

    m_pUITransformComs[7]->Set_Scaled(vSPScale.x, vSPScale.y, vSPScale.z);
    _float4 vSP_MID_Position = m_pUITransformComs[7]->Get_State_Float4(CTransform::STATE_POSITION);
    vSP_MID_Position.y = g_iWinSizeY * -0.5f + 72.f - m_fSPSize * 0.5f * (1 - fSPRatio);
    m_pUITransformComs[7]->Set_State(CTransform::STATE_POSITION, vSP_MID_Position);

    _float4 vSP_TOP_Position = m_pUITransformComs[8]->Get_State_Float4(CTransform::STATE_POSITION);
    vSP_TOP_Position.y = g_iWinSizeY * -0.5f + 100.f - m_fSPSize * (1 - fSPRatio);
    m_pUITransformComs[8]->Set_State(CTransform::STATE_POSITION, vSP_TOP_Position);


    // MP
    _float3     vMPScale = m_pUITransformComs[10]->Get_Scaled();
    _float      fMPRatio = m_fMP / m_fMaxMP;
    vMPScale.y = m_fMPSize * fMPRatio;

    if (vMPScale.y <= 0)
        vMPScale.y = 0.01f;

    m_pUITransformComs[10]->Set_Scaled(vMPScale.x, vMPScale.y, vMPScale.z);
    _float4 vMP_MID_Position = m_pUITransformComs[10]->Get_State_Float4(CTransform::STATE_POSITION);
    vMP_MID_Position.y = g_iWinSizeY * -0.5f + 72.f - m_fMPSize * 0.5f * (1 - fMPRatio);
    m_pUITransformComs[10]->Set_State(CTransform::STATE_POSITION, vMP_MID_Position);

    _float4 vMP_TOP_Position = m_pUITransformComs[11]->Get_State_Float4(CTransform::STATE_POSITION);
    vMP_TOP_Position.y = g_iWinSizeY * -0.5f + 100.f - m_fMPSize * (1 - fMPRatio);
    m_pUITransformComs[11]->Set_State(CTransform::STATE_POSITION, vMP_TOP_Position);

    return S_OK;
}

void CUI_Stat::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Stat::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Stat::Set_UIInfo()
{
    // 0 - STAT7 : HP
    CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(100.f, 190.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 50.f, g_iWinSizeY * -0.5f + 95.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    UI_DESC tDesc = {};
    tDesc.iBindTextureIndex = 9;
    tDesc.iBindTransformIndex = 0;
    m_UIDescs.emplace_back(tDesc); 

    // 1 - STAT4 : SP
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(100.f, 200.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 110.f, g_iWinSizeY * -0.5f + 100.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 10;
    tDesc.iBindTransformIndex = 1;
    m_UIDescs.emplace_back(tDesc);

    // 2 - STAT4 : MP
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(100.f, 200.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 170.f, g_iWinSizeY * -0.5f + 100.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 10;
    tDesc.iBindTransformIndex = 2;
    m_UIDescs.emplace_back(tDesc);

    // ====================================================================================================================================
    
    // 3 - HP_BOT
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(32.f, 32.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 50.5f, g_iWinSizeY * -0.5f + 43.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 2;
    tDesc.iBindTransformIndex = 3;
    m_UIDescs.emplace_back(tDesc);

    // 4 - HP_MID
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    m_fHPSize = 120.f;
    pTransform->Set_Scaled(32.f, m_fHPSize, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 50.5f, g_iWinSizeY * -0.5f + 95.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 1;
    tDesc.iBindTransformIndex = 4;
    m_UIDescs.emplace_back(tDesc);

    // 5 - HP_TOP
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(32.f, 32.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 50.5f, g_iWinSizeY * -0.5f + 146.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 0;
    tDesc.iBindTransformIndex = 5;
    m_UIDescs.emplace_back(tDesc);

    // ====================================================================================================================================

    // 6 - SP_BOT
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(32.f, 32.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 110.5f, g_iWinSizeY * -0.5f + 44.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 5;
    tDesc.iBindTransformIndex = 6;
    m_UIDescs.emplace_back(tDesc);
    
    // 7 - SP_MID
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    m_fSPSize = 72.f;
    pTransform->Set_Scaled(32.f, m_fSPSize, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 110.5f, g_iWinSizeY * -0.5f + 72.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 4;
    tDesc.iBindTransformIndex = 7;
    m_UIDescs.emplace_back(tDesc);

    // 8 - SP_TOP
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(32.f, 32.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 110.5f, g_iWinSizeY * -0.5f + 100.f, 0.f, 1.f }); // +17
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 3;
    tDesc.iBindTransformIndex = 8;
    m_UIDescs.emplace_back(tDesc);

    // ====================================================================================================================================

    // 9 - MP_BOT
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(32.f, 32.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 170.5f, g_iWinSizeY * -0.5f + 44.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 8;
    tDesc.iBindTransformIndex = 9;
    m_UIDescs.emplace_back(tDesc);

    // 10 - MP_MID
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    m_fMPSize = 72.f;
    pTransform->Set_Scaled(32.f, m_fMPSize, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 170.5f, g_iWinSizeY * -0.5f + 72.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 7;
    tDesc.iBindTransformIndex = 10;
    m_UIDescs.emplace_back(tDesc);

    // 11 - MP_TOP
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(32.f, 32.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 170.5f, g_iWinSizeY * -0.5f + 100.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 6;
    tDesc.iBindTransformIndex = 11;
    m_UIDescs.emplace_back(tDesc);
    

    // ====================================================================================================================================

    // 12 - LINE7 : HP
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(100.f, 180.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 50.f, g_iWinSizeY * -0.5f + 95.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 11;
    tDesc.iBindTransformIndex = 12;
    m_UIDescs.emplace_back(tDesc);

    // 13 - LINE4 : SP
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(100.f, 200.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 110.f, g_iWinSizeY * -0.5f + 100.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 12;
    tDesc.iBindTransformIndex = 13;
    m_UIDescs.emplace_back(tDesc);

    // 14 - LINE4 : MP
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(100.f, 200.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5 + 170.f, g_iWinSizeY * -0.5f + 100.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 12;
    tDesc.iBindTransformIndex = 14;
    m_UIDescs.emplace_back(tDesc);

    for (auto& tDesc : m_UIDescs)
        tDesc.eShader = UI_STAT;

    return S_OK;
}

HRESULT CUI_Stat::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Stat"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Stat::Bind_ShaderResources()
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

CUI_Stat* CUI_Stat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Stat* pInstance = new CUI_Stat(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CUI_Stat"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Stat::Clone(void* pArg)
{
    CUI_Stat* pInstance = new CUI_Stat(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CUI_Stat"));

        Safe_Release(pInstance);
    }

    return pInstance;
}


void CUI_Stat::Free()
{
    __super::Free();
}

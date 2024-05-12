#include "stdafx.h"
#include "UI_Loading.h"

CUI_Loading::CUI_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

void CUI_Loading::Set_Using(_bool isUsing, _int iLeafIndex)
{
    //m_isUsing = isUsing; 
    m_fAccShaderTimeDelta = 0.f;
    m_isFinish = false;

    if (true == isUsing)
    {
        m_isFadeOut = true;             
        m_isUsing = true;

        if (-1 != iLeafIndex)
            m_UIDescs[2].iBindTextureIndex = iLeafIndex;
    }
    else
    {
        m_isFadeOut = false;
        m_UIDescs[1].isRender = false;
        m_UIDescs[2].isRender = false;
    }  
}

HRESULT CUI_Loading::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Loading::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_UIInfo()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Loading::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    if (true == m_isUsing)
    {
        m_fAccShaderTimeDelta += fTimeDelta;
        if (m_fAccShaderTimeDelta >= m_fShaderTimeDelta)
        {
            if (true == m_isFadeOut)
            {
                m_isLoading = true;
                m_UIDescs[1].isRender = true;
                m_UIDescs[2].isRender = true;
            }
            else if(false == m_isLoading && false == m_isFadeOut)
            {
                m_isUsing = false;
                m_fAccShaderTimeDelta = 0.f;
            }
        }

        if (true == m_isLoading)
        {
            m_fAccLeafTime += fTimeDelta;
            if (m_fAccLeafTime >= m_fLeafTime)
            {
                m_isLoading = false;
                m_isFinish = true;
                m_fAccLeafTime = 0.f;
            }
        }
    }

    return S_OK;
}

void CUI_Loading::Late_Tick(_float fTimeDelta)
{
    if(true == m_isUsing)
    {
        __super::Late_Tick(fTimeDelta);      
    }
}

HRESULT CUI_Loading::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Loading::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Loading"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Loading::Bind_ShaderResources()
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

    if (FAILED(m_pShaderCom->Bind_RawValue("g_isFadeOut", &m_isFadeOut, sizeof(_bool))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccTimeDelta", &m_fAccShaderTimeDelta, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Loading::Set_UIInfo()
{
    // 0 - BG
    CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(g_iWinSizeX, g_iWinSizeY, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ 0.f, 0.f, 0.02f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    UI_DESC tDesc = {};
    tDesc.iBindTextureIndex = 8;
    tDesc.iBindTransformIndex = 0;
    tDesc.eShader = UI_LOADING;
    tDesc.isRender = true;
    m_UIDescs.emplace_back(tDesc);

    // 1 - Back_Yellow
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(814.f, 620.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ 0.f, 0.f, 0.01f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 9;
    tDesc.iBindTransformIndex = 1;
    tDesc.eShader = (CUI::UI_SHADER)0;
    tDesc.isRender = false;
    m_UIDescs.emplace_back(tDesc);

    // 2 - Leaf
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(764.f, 582.f, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ 0.f, 0.f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);
    
    tDesc = {};
    tDesc.iBindTextureIndex = 2;
    tDesc.iBindTransformIndex = 2;
    tDesc.eShader = UI_LEAF;
    tDesc.isRender = false;
    m_UIDescs.emplace_back(tDesc);

    return S_OK;
}

CUI_Loading* CUI_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Loading* pInstance = new CUI_Loading(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CUI_Loading"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Loading::Clone(void* pArg)
{
    CUI_Loading* pInstance = new CUI_Loading(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CUI_Loading"));

        Safe_Release(pInstance);
    }

    return pInstance;
}


void CUI_Loading::Free()
{
    __super::Free();
}
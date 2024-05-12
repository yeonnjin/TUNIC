#include "stdafx.h"
#include "UI_Interactive.h"

CUI_Interactive::CUI_Interactive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

void CUI_Interactive::Set_Using(_bool isUsing, _vector vTargetPosition)
{
    m_isUsing = isUsing;
    vTargetPosition.m128_f32[1] += 3.f;

    _vector vViewPos = XMVector3TransformCoord(vTargetPosition, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    _vector vProjPos = XMVector3TransformCoord(vViewPos, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    _float2 vProjPosition = { (XMVectorGetX(vProjPos) + 1.f) * 0.5f * 1280.f,
                                (1.f - XMVectorGetY(vProjPos)) * 0.5f * 720.f };

    m_pUITransformComs[0]->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + vProjPosition.x, g_iWinSizeY * 0.5f - vProjPosition.y, 0.8f, 1.f });
}

HRESULT CUI_Interactive::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Interactive::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (nullptr == pArg)
        return E_FAIL;

    UI_INTERACTIVE_DESC* pDesc = (UI_INTERACTIVE_DESC*)pArg;
    m_pTargetTransform = pDesc->pTargetTransform;
    
    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_UIInfo()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Interactive::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CUI_Interactive::Late_Tick(_float fTimeDelta)
{
    if (true == m_isUsing)
        __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Interactive::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Interactive::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
    return E_FAIL;
    
    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Interactive"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CUI_Interactive::Bind_ShaderResources()
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

HRESULT CUI_Interactive::Set_UIInfo()
{
    // 0 - SPACE BAR
    CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;
    
    _float fSizeX, fSizeY;

    fSizeX = m_fSize;
    fSizeY = m_fSize;
    pTransform->Set_Scaled(fSizeX, fSizeY, 1.f);
    m_pUITransformComs.emplace_back(pTransform);

    UI_DESC tDesc = {};
    tDesc.iBindTextureIndex = 0;
    tDesc.iBindTransformIndex = 0;
    tDesc.eShader = UI_SLOT;
    tDesc.isRender = true;
    m_UIDescs.emplace_back(tDesc);

    return S_OK;
}

CUI_Interactive* CUI_Interactive::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Interactive* pInstance = new CUI_Interactive(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CUI_Interactive"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Interactive::Clone(void* pArg)
{
    CUI_Interactive* pInstance = new CUI_Interactive(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CUI_Interactive"));

        Safe_Release(pInstance);
    }

    return pInstance;
}


void CUI_Interactive::Free()
{
    __super::Free();
}
#include "stdafx.h"
#include "UI_Aggro.h"

CUI_Aggro::CUI_Aggro(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

void CUI_Aggro::Set_Using(_bool isUsing)
{
    m_isUsing = isUsing;

    if (false == m_isUsing)
        return;
}

HRESULT CUI_Aggro::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Aggro::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (nullptr == pArg)
        return E_FAIL;

    UI_AGGRO_DESC* pDesc = (UI_AGGRO_DESC*)pArg;
    m_fHeight = pDesc->fHeight;
    m_pTargetTransform = pDesc->pTargetTransform;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_UIInfo()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Aggro::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    if (true == m_isUsing)
    {
        _vector vPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);
        vPosition.m128_f32[1] += m_fHeight;
        _vector vViewPos = XMVector3TransformCoord(vPosition, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
        _vector vProjPos = XMVector3TransformCoord(vViewPos, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

        _float2 vProjPosition = { (XMVectorGetX(vProjPos) + 1.f) * 0.5f * 1280.f,
                                    (1.f - XMVectorGetY(vProjPos)) * 0.5f * 720.f };

        m_pUITransformComs[0]->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + vProjPosition.x, g_iWinSizeY * 0.5f - vProjPosition.y, 0.8f, 1.f });
    }

    return S_OK;
}

void CUI_Aggro::Late_Tick(_float fTimeDelta)
{
    if (true == m_isUsing)
        __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Aggro::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Aggro::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Aggro"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Aggro::Bind_ShaderResources()
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

HRESULT CUI_Aggro::Set_UIInfo()
{
    // 0 - Aggro
    CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(m_fSize, m_fSize, 1.f);
    m_pUITransformComs.emplace_back(pTransform);

    UI_DESC tDesc = {};
    tDesc.iBindTextureIndex = 0;
    tDesc.iBindTransformIndex = 0;
    tDesc.eShader = UI_SLOT;
    tDesc.isRender = true;
    m_UIDescs.emplace_back(tDesc);

    return S_OK;
}

CUI_Aggro* CUI_Aggro::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Aggro* pInstance = new CUI_Aggro(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CUI_Aggro"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Aggro::Clone(void* pArg)
{
    CUI_Aggro* pInstance = new CUI_Aggro(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CUI_Aggro"));

        Safe_Release(pInstance);
    }

    return pInstance;
}


void CUI_Aggro::Free()
{
    __super::Free();
}
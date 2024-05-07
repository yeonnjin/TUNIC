#include "stdafx.h"
#include "UI_LockOn.h"

CUI_LockOn::CUI_LockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

void CUI_LockOn::Set_Using(_bool isUsing, CTransform* pTargetTransform)
{
    m_isUsing = isUsing;

    if (false == m_isUsing)
    {
        Safe_Release(m_pTargetTransform);
        return;
    }

    m_pTargetTransform = pTargetTransform;
    Safe_AddRef(m_pTargetTransform);
}

HRESULT CUI_LockOn::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_LockOn::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_UIInfo()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_LockOn::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    if (true == m_isUsing)
    {
        _vector vPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);
        _vector vViewPos = XMVector3TransformCoord(vPosition, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
        _vector vProjPos = XMVector3TransformCoord(vViewPos, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

        _float2 vProjPosition = { (XMVectorGetX(vProjPos) + 1.f) * 0.5f * 1280.f,
                                    (1.f - XMVectorGetY(vProjPos)) * 0.5f * 720.f };

        m_pUITransformComs[0]->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + vProjPosition.x, g_iWinSizeY * 0.5f - vProjPosition.y, 0.8f, 1.f });
    }

    return S_OK;
}

void CUI_LockOn::Late_Tick(_float fTimeDelta)
{
    if (true == m_isUsing)
        __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_LockOn::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_LockOn::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LockOn"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_LockOn::Bind_ShaderResources()
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

HRESULT CUI_LockOn::Set_UIInfo()
{
    // 0 - LockOn
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

CUI_LockOn* CUI_LockOn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_LockOn* pInstance = new CUI_LockOn(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CUI_LockOn"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_LockOn::Clone(void* pArg)
{
    CUI_LockOn* pInstance = new CUI_LockOn(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CUI_LockOn"));

        Safe_Release(pInstance);
    }

    return pInstance;
}


void CUI_LockOn::Free()
{
    __super::Free();

    Safe_Release(m_pTargetTransform);
}
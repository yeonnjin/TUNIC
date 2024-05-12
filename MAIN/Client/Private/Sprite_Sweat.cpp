#include "stdafx.h"
#include "Sprite_Sweat.h"

#include "Player.h"

CSprite_Sweat::CSprite_Sweat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CSprite{ pDevice, pContext }
{
}

void CSprite_Sweat::Set_Using(_bool isUsing)
{
    m_isUsing = isUsing;
    m_UIDescs[0].isRender = isUsing;
}

HRESULT CSprite_Sweat::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSprite_Sweat::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_UIInfo()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite_Sweat::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    if (true == m_isUsing)
    {
        // 위치 설정
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
        CTransform* pTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag));
        _vector vTargetPosition = pTransform->Get_State_Vector(CTransform::STATE_POSITION);
        vTargetPosition.m128_f32[1] += 3.f;
        _vector vViewPos = XMVector3TransformCoord(vTargetPosition, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
        _vector vProjPos = XMVector3TransformCoord(vViewPos, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

        _float2 vProjPosition = { (XMVectorGetX(vProjPos) + 1.f) * 0.5f * 1280.f,
                                    (1.f - XMVectorGetY(vProjPos)) * 0.5f * 720.f };

        m_pUITransformComs[0]->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + vProjPosition.x, g_iWinSizeY * 0.5f - vProjPosition.y, 0.8f, 1.f });

        // 이미지 설정
        m_fAccTickTime += fTimeDelta;
        if (m_fAccTickTime >= m_fTickTime)
        {
            ++m_iCurIndex;
            m_fAccTickTime = 0.f;
            if (m_iCurIndex > m_iMaxIndex)
            {
                m_iCurIndex = 0;
                m_isUsing = false;
            }
        }

        m_UIDescs[0].iBindTextureIndex = m_iCurIndex;
    }

    return S_OK;
}

void CSprite_Sweat::Late_Tick(_float fTimeDelta)
{
    if (true == m_isUsing)
        __super::Late_Tick(fTimeDelta);
}

HRESULT CSprite_Sweat::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite_Sweat::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sprite_Sweat"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite_Sweat::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite_Sweat::Set_UIInfo()
{
    // 0 - SWEAT
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

CSprite_Sweat* CSprite_Sweat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSprite_Sweat* pInstance = new CSprite_Sweat(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CSprite_Sweat"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSprite_Sweat::Clone(void* pArg)
{
    CSprite_Sweat* pInstance = new CSprite_Sweat(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CSprite_Sweat"));

        Safe_Release(pInstance);
    }

    return pInstance;
}


void CSprite_Sweat::Free()
{
    __super::Free();
}
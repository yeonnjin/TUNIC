#include "stdafx.h"
#include "Sprite_Attack.h"

CSprite_Attack::CSprite_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CSprite{ pDevice, pContext }
{
}

void CSprite_Attack::Set_Using(_bool isUsing, CTransform* pTargetTransform)
{
    m_isUsing = isUsing;
    m_UIDescs[0].isRender = isUsing;

    m_pTargetTransform = pTargetTransform;
    Safe_AddRef(m_pTargetTransform);
}

HRESULT CSprite_Attack::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSprite_Attack::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite_Attack::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    if (true == m_isUsing)
    {
        // 위치 설정
        _vector vTargetPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);
        vTargetPosition.m128_f32[1] += 0.5f;
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
                m_isDead = true;
            }
        }

        m_UIDescs[0].iBindTextureIndex = m_iCurIndex;
    }

    return S_OK;
}

void CSprite_Attack::Late_Tick(_float fTimeDelta)
{
    if (true == m_isUsing)
        __super::Late_Tick(fTimeDelta);
}

HRESULT CSprite_Attack::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite_Attack::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSprite_Attack::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

void CSprite_Attack::Free()
{
    __super::Free();

    if (nullptr != m_pTargetTransform)
        Safe_Release(m_pTargetTransform);
}

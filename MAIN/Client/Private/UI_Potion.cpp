#include "stdafx.h"
#include "UI_Potion.h"

#include "Player.h"

CUI_Potion::CUI_Potion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

void CUI_Potion::Use_Potion()
{
    if (0 >= m_iNumPotion)
        return;

    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));

    _uint iHP = pPlayer->Get_HP();
    _uint iMaxHP = pPlayer->Get_MaxHP();
    // 이미 풀일 때 : 아이템 사용 실패
    if (iMaxHP <= iHP)
        return;
    // 회복 가능한 양보다 회복될 양이 많을 때 : 최대 게이지로
    else if (m_fPlusHP >= iMaxHP - iHP)
    {
        pPlayer->Set_HP(iMaxHP - iHP);
    }
    // 일반 상태일 때 : + 회복양
    else
    {
        pPlayer->Set_HP(m_fPlusHP);
    }

    m_iNumPotion -= 1;

    for (size_t i = 0; i < m_iMaxPotion - m_iNumPotion; i++)
    {
        m_UIDescs[i].iBindTextureIndex = 1;
    }
}

void CUI_Potion::Add_Potion()
{
    if (m_iMaxPotion <= m_iNumPotion)
        return;

    m_iNumPotion += 1;

    for (size_t i = 1; i < m_iNumPotion + 1; i++)
    {
        m_UIDescs[m_iMaxPotion - i].iBindTextureIndex = 0;
    }
}

HRESULT CUI_Potion::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Potion::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_UIInfo()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Potion::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CUI_Potion::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Potion::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Potion::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Potion"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Potion::Bind_ShaderResources()
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

HRESULT CUI_Potion::Set_UIInfo()
{
    // POSITION
    _float fMinSizeX = 1146.f;
    _float fDistanceX = 50.f;
    _float fY = 147.f;

    for (size_t i = 0; i < m_iMaxPotion; i++)
    {
        m_vPositions[i] = { fMinSizeX + i * fDistanceX, fY };
    }

    // 0 - Potion0
    CTransform * pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(m_fSize, m_fSize, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + m_vPositions[0].x, g_iWinSizeY * 0.5f - m_vPositions[0].y, 0.f, 1.f});
    m_pUITransformComs.emplace_back(pTransform);

    UI_DESC tDesc = {};
    tDesc.iBindTextureIndex = 0;
    tDesc.iBindTransformIndex = 0;
    tDesc.eShader = UI_STAT;
    tDesc.isRender = true;
    m_UIDescs.emplace_back(tDesc);

    // 1 - Potion1
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    pTransform->Set_Scaled(m_fSize, m_fSize, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + m_vPositions[1].x, g_iWinSizeY * 0.5f - m_vPositions[1].y, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 0;
    tDesc.iBindTransformIndex = 1;
    tDesc.eShader = UI_STAT;
    tDesc.isRender = true;
    m_UIDescs.emplace_back(tDesc);

    return S_OK;
}

CUI_Potion* CUI_Potion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Potion* pInstance = new CUI_Potion(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CUI_Potion"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Potion::Clone(void* pArg)
{
    CUI_Potion* pInstance = new CUI_Potion(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CUI_Potion"));

        Safe_Release(pInstance);
    }


    return pInstance;
}


void CUI_Potion::Free()
{
    __super::Free();
}
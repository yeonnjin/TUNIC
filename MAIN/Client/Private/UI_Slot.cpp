#include "stdafx.h"
#include "UI_Slot.h"

#include "Player.h"

CUI_Slot::CUI_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI{ pDevice, pContext }
{
}

void CUI_Slot::Set_Slot(SLOT eSlot, CPlayer::WEAPON eWeapon)
{
    // 기존에 있을 때 : 슬롯 변경 (UI, 변수)
    for (size_t i = 0; i < SLOT_END; i++)
    {
        if (m_eSlotWeapon[i] == eWeapon && i != eSlot)
        {
            m_eSlotWeapon[eSlot] = eWeapon;
            m_eSlotWeapon[i] = CPlayer::WEAPON_END;
            m_UIDescs[eSlot + 1].iBindTextureIndex = m_UIDescs[i + 1].iBindTextureIndex;
            m_UIDescs[eSlot + 1].isRender = true;
            m_UIDescs[i + 1].isRender = false;
            return;
        }
    }

    // 새로 추가할 때
    m_eSlotWeapon[eSlot] = eWeapon;

    m_UIDescs[eSlot + 1].iBindTextureIndex = Set_WeaponTextureIndex(eWeapon);
    m_UIDescs[eSlot + 1].isRender = true;
}

HRESULT CUI_Slot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Slot::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Set_UIInfo()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Slot::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CUI_Slot::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Slot::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Slot::Add_Components()
{
    // SHADER, VIBUFFER
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Slot"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Slot::Bind_ShaderResources()
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

HRESULT CUI_Slot::Set_UIInfo()
{
    // POSITION
    _uint iMaxSlot = 3;
    _float fMinSizeX = 1006.5f;
    _float fDistanceX = 103.f;
    _float fY = 60.5f;

    for (size_t i = 0; i < SLOT_END; i++)
    {
        m_vPositions[i] = { fMinSizeX + i * fDistanceX, fY };
    }

    // 0 - BG
    CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    _float fSizeX, fSizeY;

    fSizeX = 365.f;
    fSizeY = 183.f;
    pTransform->Set_Scaled(fSizeX, fSizeY, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + 1143.5f, g_iWinSizeY * 0.5f - 106.5f, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    UI_DESC tDesc = {};
    tDesc.iBindTextureIndex = 0;
    tDesc.iBindTransformIndex = 0;
    tDesc.eShader = UI_SLOT;
    tDesc.isRender = true;
    m_UIDescs.emplace_back(tDesc);

    // 1 - Slot1
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    fSizeX = m_fSize;
    fSizeY = m_fSize;
    pTransform->Set_Scaled(fSizeX, fSizeY, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + m_vPositions[SLOT_J].x, g_iWinSizeY * 0.5f - m_vPositions[SLOT_J].y, 0.f, 1.f});
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 1;
    tDesc.iBindTransformIndex = 1;
    tDesc.eShader = UI_STAT;
    tDesc.isRender = false;
    m_UIDescs.emplace_back(tDesc);

    // 2 - Slot2
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    fSizeX = m_fSize;
    fSizeY = m_fSize;
    pTransform->Set_Scaled(fSizeX, fSizeY, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + m_vPositions[SLOT_K].x, g_iWinSizeY * 0.5f - m_vPositions[SLOT_K].y, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 2;
    tDesc.iBindTransformIndex = 2;
    tDesc.eShader = UI_STAT;
    tDesc.isRender = false;
    m_UIDescs.emplace_back(tDesc);  

    // 3 - Slot3
    pTransform = CTransform::Create(m_pDevice, m_pContext);
    if (nullptr == pTransform)
        return E_FAIL;

    fSizeX = m_fSize;
    fSizeY = m_fSize;
    pTransform->Set_Scaled(fSizeX, fSizeY, 1.f);
    pTransform->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + m_vPositions[SLOT_L].x, g_iWinSizeY * 0.5f - m_vPositions[SLOT_L].y, 0.f, 1.f });
    m_pUITransformComs.emplace_back(pTransform);

    tDesc = {};
    tDesc.iBindTextureIndex = 3;
    tDesc.iBindTransformIndex = 3;
    tDesc.eShader = UI_STAT;
    tDesc.isRender = false;
    m_UIDescs.emplace_back(tDesc);

    return S_OK; 
}

_uint CUI_Slot::Set_WeaponTextureIndex(CPlayer::WEAPON eWeapon)
{
    switch (eWeapon)
    {
    case Client::CPlayer::WEAPON_STICK:
        return 1;
    case Client::CPlayer::WEAPON_SWORD:
        return 2;
    case Client::CPlayer::WEAPON_WAND:
        return 3;
    default:
        break;
    }
}

CUI_Slot* CUI_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Slot* pInstance = new CUI_Slot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CUI_Slot"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Slot::Clone(void* pArg)
{
    CUI_Slot* pInstance = new CUI_Slot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CUI_Slot"));

        Safe_Release(pInstance);
    }

    return pInstance;
}


void CUI_Slot::Free()
{
    __super::Free();
}
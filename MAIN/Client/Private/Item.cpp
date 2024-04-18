#include "stdafx.h"
#include "Item.h"

CItem::CItem()
{
}

HRESULT CItem::Initialize()
{
    return S_OK;
}

void CItem::Tick(_float fTimeDelta)
{
}

void CItem::Set_TextureInfo()
{
    switch (m_eItem)
    {
    case Client::CItem::ITEM_CUBIC:
        m_vTexturePos = { 0, 0 };
        break;
    case Client::CItem::ITEM_BREAK:
        m_vTexturePos = { 0, 1 };
        break;
    case Client::CItem::ITEM_SHIELD:
        m_vTexturePos = { 1, 0 };
        break;
    case Client::CItem::ITEM_DASH:
        m_vTexturePos = { 1, 1 };
        break;
    case Client::CItem::ITEM_HP:
        m_vTexturePos = { 1, 0 };
        break;
    case Client::CItem::ITEM_MP:
        m_vTexturePos = { 1, 1 };
        break;
    case Client::CItem::ITEM_BOMB:
        m_vTexturePos = { 1, 2 };
        break;
    case Client::CItem::ITEM_ICE:
        m_vTexturePos = { 1, 3 };
        break;
    case Client::CItem::ITEM_STICK:
        m_vTexturePos = { 2, 1 };
        break;
    case Client::CItem::ITEM_SWORD:
        m_vTexturePos = { 2, 2 };
        break;
    case Client::CItem::ITEM_WAND:
        m_vTexturePos = { 2, 3 };
        break;
    case Client::CItem::ITEM_END:
        break;
    default:
        break;
    }
}

CItem* CItem::Create()
{
    CItem* pInstance = new CItem();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CItem"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CItem::Free()
{
}

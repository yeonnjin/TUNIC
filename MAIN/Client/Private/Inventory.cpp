#include "stdafx.h"
#include "Inventory.h"

#include "UI_Inventory.h"
#include "UI_Item.h"
#include "UI_Slot.h"
#include "UI_Obtain.h"

#include "Player.h"

CInventory::CInventory()
    : m_pGameInstance{ CGameInstance::Get_Instance() }
{
    Safe_AddRef(m_pGameInstance);
}

_uint CInventory::Get_Weapon(_uint iKey)
{
    CUI_Slot::SLOT eSlot;
    if (DIK_J == iKey)
        eSlot = CUI_Slot::SLOT_J;
    else if(DIK_K == iKey)
        eSlot = CUI_Slot::SLOT_K;
    else if (DIK_L == iKey)
        eSlot = CUI_Slot::SLOT_L;

    return m_pUISlot->Get_Weapon(eSlot);
}

void CInventory::Add_Item(CItem* pItem)
{
    CItem::ITEM_TYPE eType = pItem->Get_ItemType();
    CItem::ITEM      eItem = pItem->Get_Item();

    // 이미 있는 아이템인 경우 : 기존 아이템의 개수 증가, 
    // TODO: 숫자 카운트 증가

    m_pUIObtain->Set_Using(true, eItem);

    for (size_t i = 0; i < m_iNumItems[eType].size(); i++)
    {
        if (eItem == m_Items[eType][i]->Get_Item())
        {
            m_Items[eType][i]->Plus_Count(true);
            m_iNumItems[eType][i] += 1;
            Safe_Release(pItem);
            return;
        }
    }

    // 새로운 아이템인 경우 : 사이즈 내에서 가능

    if (m_iMaxItem > m_iNumItems[eType].size())
    {
        m_Items[eType][m_iNumItems[eType].size()] = pItem;
        CUI_Item::UIITEM_DESC tDesc{};
        tDesc.iTextureIndex = eItem;
        tDesc.vPosition = m_pUIInventory->Get_Position((_uint)eType, m_iNumItems[eType].size());
        CUI_Item* pUIItem = dynamic_cast<CUI_Item*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_UI_Item"), &tDesc));
        if (nullptr == pUIItem)
            return;

        m_pUIItems[eType].emplace_back(pUIItem);
        m_iNumItems[eType].push_back(1);
    }
}

void CInventory::Select_Item()
{
    // 방향키를 눌렀을 때, 해당 방향에 아이템이 있으면 이동
    if (true == m_pGameInstance->Get_DIKeyState(DIK_UP, KEY_DOWN) && 0 < m_iSelectRow)
    {
        for (size_t i = 1; i < m_iSelectRow + 1; i++)
        {
            if (nullptr != m_Items[m_iSelectRow - i][m_iSelectColumn])
            {
                m_iSelectRow -= i;
                break;
            }
        }
    }
    else if (true == m_pGameInstance->Get_DIKeyState(DIK_DOWN, KEY_DOWN) && CItem::TYPE_END - 1 > m_iSelectRow)
    {
        for (size_t i = 1; i < CItem::TYPE_END - m_iSelectRow; i++)
        {
            if (nullptr != m_Items[m_iSelectRow + i][m_iSelectColumn])
            {
                m_iSelectRow += i;
                break;
            }
        }
    }
    else if (true == m_pGameInstance->Get_DIKeyState(DIK_LEFT, KEY_DOWN))
    {
        if(0 < m_iSelectColumn)
        {
            for (size_t i = 1; i < m_iSelectColumn + 1; i++)
            {
                if (nullptr != m_Items[m_iSelectRow][m_iSelectColumn - i])
                {
                    m_iSelectColumn -= i;
                    break;
                }
            }
        }
        // 맨 왼쪽 아이템일 때, 윗 줄 마지막 아이템으로 이동
        else if(0 == m_iSelectColumn && 0 < m_iSelectRow)
        {
            for (size_t i = m_iMaxItem - 1 ; i >= 0; i--)
            {
                if (nullptr != m_Items[m_iSelectRow - 1][i])
                {
                    m_iSelectRow -= 1;
                    m_iSelectColumn = i;
                    break;
                }
            }
        }
    }
    else if (true == m_pGameInstance->Get_DIKeyState(DIK_RIGHT, KEY_DOWN))
    {
        if(m_iMaxItem - 1 > m_iSelectColumn)
        {
            for (size_t i = 1; i < m_iMaxItem - m_iSelectColumn; i++)
            {
                if (nullptr != m_Items[m_iSelectRow][m_iSelectColumn + i])
                {
                    m_iSelectColumn += i;
                    break;
                }
            }
        }
        // 맨 오른쪽 아이템일 때, 아랫 줄 첫번째 아이템으로 이동
        else if (m_iMaxItem - 1 == m_iSelectColumn && CItem::TYPE_END - 1 > m_iSelectRow)
        {
            for (size_t i = 0 ; i <= m_iMaxItem - 1; i++)
            {
                if (nullptr != m_Items[m_iSelectRow + 1][i])
                {
                    m_iSelectRow += 1;
                    m_iSelectColumn = i;
                    break;
                }
            }
        }
    }

    m_pUIInventory->Set_Select(m_iSelectRow, m_iSelectColumn);

    // 아이템 사용
    // TYPE_USE
    if (true == m_pGameInstance->Get_DIKeyState(DIK_RETURN, KEY_DOWN) && m_iSelectRow == CItem::TYPE_USE)
    {
        if (S_OK == m_Items[m_iSelectRow][m_iSelectColumn]->Use_Item())
        {
            m_iNumItems[m_iSelectRow][m_iSelectColumn] -= 1;
            if (0 == m_iNumItems[m_iSelectRow][m_iSelectColumn])
            {
                // v.erase(v.begin() + 5);

                m_iNumItems[m_iSelectRow].erase(m_iNumItems[m_iSelectRow].begin() + m_iSelectColumn);

                auto iter = m_pUIItems[m_iSelectRow].begin() + m_iSelectColumn;
                Safe_Release(*iter);
                m_pUIItems[m_iSelectRow].erase(m_pUIItems[m_iSelectRow].begin() + m_iSelectColumn);

                auto it = m_Items[m_iSelectRow].begin() + m_iSelectColumn;
                Safe_Release(*it);

                // 뒤에 아이템들 당겨오기
                for (size_t i = m_iSelectColumn; i < m_iMaxItem; i++)
                {
                    if (nullptr == m_Items[m_iSelectRow][i + 1])
                        break;
                    else
                    {
                        m_Items[m_iSelectRow][i] = m_Items[m_iSelectRow][i + 1];
                        m_Items[m_iSelectRow][i + 1] = nullptr;
                        m_pUIItems[m_iSelectRow][i]->Set_Position(m_pUIInventory->Get_Position(m_iSelectRow, i));
                    }
                }
            }
        }
    }
    // TYPE_WEAPON
    // SLOT_J
    else if (true == m_pGameInstance->Get_DIKeyState(DIK_J, KEY_DOWN) && m_iSelectRow == CItem::TYPE_WEAPON)
    {
        m_pUISlot->Set_Slot(CUI_Slot::SLOT_J, (CPlayer::WEAPON)Set_WeaponIndex(m_Items[m_iSelectRow][m_iSelectColumn]->Get_Item()));
    }
    // SLOT_K
    else if (true == m_pGameInstance->Get_DIKeyState(DIK_K, KEY_DOWN) && m_iSelectRow == CItem::TYPE_WEAPON)
    {
        m_pUISlot->Set_Slot(CUI_Slot::SLOT_K, (CPlayer::WEAPON)Set_WeaponIndex(m_Items[m_iSelectRow][m_iSelectColumn]->Get_Item()));
    }
    // SLOT_L
    else if (true == m_pGameInstance->Get_DIKeyState(DIK_L, KEY_DOWN) && m_iSelectRow == CItem::TYPE_WEAPON)
    {
        m_pUISlot->Set_Slot(CUI_Slot::SLOT_L, (CPlayer::WEAPON)Set_WeaponIndex(m_Items[m_iSelectRow][m_iSelectColumn]->Get_Item()));
    }
}

HRESULT CInventory::Initialize()
{
    m_pUIInventory = dynamic_cast<CUI_Inventory*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Inventory")));
    Safe_AddRef(m_pUIInventory);

    m_pUISlot = dynamic_cast<CUI_Slot*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Slot")));
    Safe_AddRef(m_pUISlot);

    m_pUIObtain = dynamic_cast<CUI_Obtain*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Obtain")));
    Safe_AddRef(m_pUIObtain);

    for (size_t i = 0; i < CItem::TYPE_END; i++)
    {
        m_Items[i].resize(m_iMaxItem, nullptr);
    }

    /*for (size_t i = 0; i < CItem::TYPE_END; i++)
    {
        for (auto& pItem : m_Items[i])
            pItem = CItem::Create();
    }*/

    return S_OK;
}

void CInventory::Tick(_float fTimeDelta)
{
    // 사용 체크
    m_pUIInventory->Set_Using(m_isUsing);

    // 선택 박스
    if(true == m_isUsing)
    {
        Select_Item();
        
        for(size_t i = 0 ; i < CItem::TYPE_END; i++)
        {
            for (auto& pItem : m_pUIItems[i])
                pItem->Late_Tick(fTimeDelta);
        }
    }
}

_uint CInventory::Set_WeaponIndex(CItem::ITEM eItem)
{
    switch (eItem)
    {
    case Client::CItem::ITEM_STICK:
        return CPlayer::WEAPON_STICK;
    case Client::CItem::ITEM_SWORD:
        return CPlayer::WEAPON_SWORD;
    case Client::CItem::ITEM_WAND:
        return CPlayer::WEAPON_WAND;
    default:
        break;
    }
}

CInventory* CInventory::Create()
{
    CInventory* pInstance = new CInventory();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CInventory"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInventory::Free()
{
    for (size_t i = 0; i < CItem::TYPE_END; i++)
    {
        for (auto& pItem : m_Items[i])
            Safe_Release(pItem);

        m_Items[i].clear();
    }

    for (size_t i = 0; i < CItem::TYPE_END; i++)
    {
        for (auto& pUIItem : m_pUIItems[i])
            Safe_Release(pUIItem);

        m_pUIItems[i].clear();
    }

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pUIInventory);
    Safe_Release(m_pUISlot);
    Safe_Release(m_pUIObtain);
}

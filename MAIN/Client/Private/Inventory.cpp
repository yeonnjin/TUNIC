#include "stdafx.h"
#include "Inventory.h"

#include "UI_Inventory.h"
#include "UI_Item.h"
#include "UI_Slot.h"
#include "UI_Obtain.h"

#include "Item.h"

CInventory::CInventory()
    : m_pGameInstance{ CGameInstance::Get_Instance() }
{
    Safe_AddRef(m_pGameInstance);
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

        m_pUIItems.emplace_back(pUIItem);
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
        else if (m_iMaxItem - 1 == m_iSelectColumn && CItem::TYPE_END - 1
            
            > m_iSelectRow)
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
        
        for (auto& pItem : m_pUIItems)
            pItem->Late_Tick(fTimeDelta);
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

    for (auto& pUIItem : m_pUIItems)
        Safe_Release(pUIItem);

    m_pUIItems.clear();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pUIInventory);
    Safe_Release(m_pUISlot);
    Safe_Release(m_pUIObtain);
}

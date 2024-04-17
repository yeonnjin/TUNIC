#include "stdafx.h"
#include "Inventory.h"

#include "UI_Inventory.h"
#include "UI_Slot.h"

CInventory::CInventory()
    : m_pGameInstance{ CGameInstance::Get_Instance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CInventory::Initialize()
{
    m_pUIInventory = dynamic_cast<CUI_Inventory*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Inventory")));
    Safe_AddRef(m_pUIInventory);

    m_pUISlot = dynamic_cast<CUI_Slot*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Slot")));
    Safe_AddRef(m_pUISlot);

    return S_OK;
}

void CInventory::Tick(_float fTimeDelta)
{
    // 사용 체크
    m_pUIInventory->Set_Using(m_isUsing);


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
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pUIInventory);
    Safe_Release(m_pUISlot);
}

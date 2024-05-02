#include "stdafx.h"
#include "Level_Menu.h"

#include "GameInstance.h"
#include "Level_Loading.h"

CLevel_Menu::CLevel_Menu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Menu::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI()))
        return E_FAIL;

    return S_OK;
}

void CLevel_Menu::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (true == m_pGameInstance->Get_DIKeyState(DIK_RCONTROL, KEY_DOWN))
    {
        if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BEACH))))
            return;
    }
}

HRESULT CLevel_Menu::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    SetWindowText(g_hWnd, TEXT("LEVEL : MENU"));

    return S_OK;
}

HRESULT CLevel_Menu::Ready_Layer_BackGround(const wstring& strLayerTag)
{
    /*if (FAILED(m_pGameInstance->Add_Clone(LEVEL_MENU, strLayerTag, TEXT("Prototype_GameObject_BackGround"))))
        return E_FAIL;*/

    return S_OK;
}

HRESULT CLevel_Menu::Ready_Layer_UI()
{
    //if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_UI_Stat"), TEXT("Prototype_GameObject_UI_Stat"))))
    //    return E_FAIL;

    //if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_UI_Inventory"), TEXT("Prototype_GameObject_UI_Inventory"))))
    //    return E_FAIL;

    //if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_UI_Slot"), TEXT("Prototype_GameObject_UI_Slot"))))
    //    return E_FAIL;

    //if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_UI_Obtain"), TEXT("Prototype_GameObject_UI_Obtain"))))
    //    return E_FAIL;

    return S_OK;
}

CLevel_Menu* CLevel_Menu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Menu* pInstance = new CLevel_Menu(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Create : CLevel_Menu"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Menu::Free()
{
    __super::Free();
}

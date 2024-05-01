#include "stdafx.h"
#include "Level_Boss.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Map.h"

#include "Player.h"
#include "Monster.h"

CLevel_Boss::CLevel_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Boss::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera()))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BOSS, TEXT("Layer_Editor"), TEXT("Prototype_GameObject_Editor"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Boss::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (GetKeyState(VK_RETURN) & 0x8000)
    {
        if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BEACH))))
            return;
    }
}

HRESULT CLevel_Boss::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    SetWindowText(g_hWnd, TEXT("LEVEL : BOSS"));

    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Map(const wstring& strLayerTag)
{
    CMap::MAP_DESC tDesc = {};
    tDesc.isRotation = false;
    tDesc.vPosition = _float3(0.f, 0.f, 0.f);
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Map_Librarian";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    tDesc.strModelComTag = wstr;
    tDesc.eLevel = LEVEL_BOSS;
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BOSS, strLayerTag, TEXT("Prototype_GameObject_Map"), &tDesc)))
        return E_FAIL;
}

HRESULT CLevel_Boss::Ready_Layer_Monster(const wstring& strLayerTag)
{
    // BOSS_LIBRARIAN
    CMonster::Monster_Desc tDesc = {};
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Boss_Librarian";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    tDesc.strModelComTag = wstr;
    tDesc.eLevel = LEVEL_BOSS;
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BOSS, strLayerTag, TEXT("Prototype_GameObject_Monster_Librarian"), &tDesc)))
    	return E_FAIL;

    //// MONSTER_BAT
    //tDesc = {};
    //_char szModelTag1[MAX_PATH] = "Prototype_Component_Model_Monster_Bat";
    //wstring wstr1(&szModelTag1[0], &szModelTag1[MAX_PATH]);
    //tDesc.strModelComTag = wstr1;
    //tDesc.eLevel = LEVEL_BOSS;
    //if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BOSS, strLayerTag, TEXT("Prototype_GameObject_Monster_Bat"), &tDesc)))
    //	return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_BackGround(const wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BOSS, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_UI()
{
    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Player()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
    pPlayer->Set_Level(LEVEL_BOSS);
    if (FAILED(pPlayer->Set_Navigation(LEVEL_BOSS)))
        return E_FAIL;
}

HRESULT CLevel_Boss::Ready_Layer_Camera()
{
    m_pGameInstance->Change_Camera(TEXT("Camera_Follow"));
    m_pGameInstance->Set_Camera_Level(LEVEL_BOSS);

    return S_OK;
}

CLevel_Boss* CLevel_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Boss* pInstance = new CLevel_Boss(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Create : CLevel_Boss"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Boss::Free()
{
    __super::Free();
}

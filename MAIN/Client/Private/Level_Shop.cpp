#include "stdafx.h"
#include "Level_Shop.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Map.h"
#include "Sky.h"

#include "Player.h"
#include "Camera_Follow.h"

#include "UI_Loading.h"
#include "Trigger_Map.h"

CLevel_Shop::CLevel_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

void CLevel_Shop::Set_NextLevel(LEVEL eNextLevel)
{
    m_isNext = true;
    CUI_Loading* pUILoading = dynamic_cast<CUI_Loading*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Loading")));
    pUILoading->Set_Using(true, 3);

    m_eNextLevel = eNextLevel;
}

HRESULT CLevel_Shop::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI()))
        return E_FAIL;

    if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Object()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera()))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SHOP, TEXT("Layer_Editor"), TEXT("Prototype_GameObject_Editor"))))
        return E_FAIL;

    m_pGameInstance->PlayBGM(TEXT("BGM_Shop_Intro.wav"), 0.1f, false);

    return S_OK;
}

void CLevel_Shop::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    static _bool isEnd = false;
    if (false == isEnd && false == m_pGameInstance->Sound_isPlaying(CSound_Manager::BGM))
    {
        m_pGameInstance->PlayBGM(TEXT("BGM_Shop_Loop.wav"));
        isEnd = true;
    }

    if (true == m_pGameInstance->Get_DIKeyState(DIK_RCONTROL, KEY_DOWN))
    {
        CUI_Loading* pUILoading = dynamic_cast<CUI_Loading*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Loading")));
        pUILoading->Set_Using(true);

        if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_PUZZLE))))
            return;
    }

    if (true == m_isNext)
    {
        CUI_Loading* pUILoading = dynamic_cast<CUI_Loading*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Loading")));
        if (true == pUILoading->Get_isFinish())
        {
            if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, m_eNextLevel))))
                return;
        }
    }
}

HRESULT CLevel_Shop::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    SetWindowText(g_hWnd, TEXT("LEVEL : SHOP"));

    return S_OK;
}

HRESULT CLevel_Shop::Ready_Layer_BackGround(const wstring& strLayerTag)
{
    CSky::SKY_DESC tDesc{};
    tDesc.strSkyComTag = TEXT("Prototype_Component_Texture_Sky_Shop");

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SHOP, strLayerTag, TEXT("Prototype_GameObject_Sky"), &tDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Shop::Ready_Layer_UI()
{
    return S_OK;
}

HRESULT CLevel_Shop::Ready_Layer_NPC(const wstring& strLayerTag)
{
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SHOP, strLayerTag, TEXT("Prototype_GameObject_NPC_Merchant"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Shop::Ready_Layer_Map(const wstring& strLayerTag)
{
    CMap::MAP_DESC tDesc = {};
    tDesc.isRotation = false;
    tDesc.vPosition = _float3(0.f, 0.f, 0.f);
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Map_Shop";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    tDesc.strModelComTag = wstr;
    tDesc.eLevel = LEVEL_SHOP;
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SHOP, strLayerTag, TEXT("Prototype_GameObject_Map"), &tDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Shop::Ready_Layer_Player()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
    pPlayer->Set_Level(LEVEL_SHOP);
    if (FAILED(pPlayer->Set_Navigation(LEVEL_SHOP)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Shop::Ready_Layer_Object()
{
    // TRIGGER
    CTrigger_Map::TRIGGER_MAP_DESC tTriggerDesc{};
    tTriggerDesc.vSize = { 2.f, 2.f, 2.f };
    tTriggerDesc.vPosition = _vector{ 0.f, 16.7f, 1.8f, 1.f };
    tTriggerDesc.eCurLevel = LEVEL_SHOP;
    tTriggerDesc.eNextLevel = LEVEL_BEACH;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_SHOP, TEXT("Layer_Trigger_Map"), TEXT("Prototype_GameObject_Object_Trigger_Map"), &tTriggerDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Shop::Ready_Layer_Camera()
{
    m_pGameInstance->Change_Camera(TEXT("Camera_Follow"));
    m_pGameInstance->Set_Camera_Level(LEVEL_SHOP);

    CCamera_Follow* pCamera = dynamic_cast<CCamera_Follow*>(m_pGameInstance->Get_Camera(TEXT("Camera_Follow")));
    pCamera->Set_EnterShop();
    /*CTransform* pCameraTransform = dynamic_cast<CTransform*>(pCamera->Get_Component(g_strTransformTag));

    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag));
    _vector vPlayerPosition = pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);

    _vector vCamPosition = { vPlayerPosition.m128_f32[0], vPlayerPosition.m128_f32[1] + 12.8f, vPlayerPosition.m128_f32[2] - 13.f, 1.f };
    pCameraTransform->Set_State(CTransform::STATE_POSITION, vCamPosition);
    pCameraTransform->Look_At(vPlayerPosition);*/

    return S_OK;
}

CLevel_Shop* CLevel_Shop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Shop* pInstance = new CLevel_Shop(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Create : CLevel_Shop"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Shop::Free()
{
    __super::Free();

    m_pGameInstance->Stop_Sound(CSound_Manager::BGM);
}

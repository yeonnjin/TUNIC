#include "stdafx.h"
#include "Level_Menu.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Player.h"

#include "Camera.h"
#include "Camera_Free.h"
#include "Camera_Follow.h"
#include "Camera_LockOn.h"

#include "UI_Loading.h"
#include "UI_Menu.h"

CLevel_Menu::CLevel_Menu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Menu::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Effect()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera()))
        return E_FAIL;

    return S_OK;
}

void CLevel_Menu::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (true == m_pGameInstance->Get_DIKeyState(DIK_RETURN, KEY_DOWN))
    {
        //Layer_UI_Menu
        m_isNext = true;
        CUI_Loading* pUILoading = dynamic_cast<CUI_Loading*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Loading")));
        pUILoading->Set_Using(true, 1);

        CUI_Menu* pUIMenu = dynamic_cast<CUI_Menu*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Menu")));
        pUIMenu->Set_Index(8);
        //pUIMenu->Set_Using(false);
    }

    if (true == m_isNext)
    {
        CUI_Loading* pUILoading = dynamic_cast<CUI_Loading*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Loading")));
        if (true == pUILoading->Get_isFadeOut())
        {
            CUI_Menu* pUIMenu = dynamic_cast<CUI_Menu*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Menu")));
            pUIMenu->Set_Using(false);
        }

        if (true == pUILoading->Get_isFinish())
        {         
            if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BEACH))))
                return;
        }
    }
}

HRESULT CLevel_Menu::Render()
{
    if (FAILED(__super::Render()))


        return E_FAIL;

    SetWindowText(g_hWnd, TEXT("LEVEL : MENU"));

    return S_OK;
}

void CLevel_Menu::Set_NextLevel(LEVEL eNextLevel)
{
    CUI_Loading* pUILoading = dynamic_cast<CUI_Loading*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Loading")));
    pUILoading->Set_Using(true);

    if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eNextLevel))))
        return;
}

HRESULT CLevel_Menu::Ready_Lights()
{
    LIGHT_DESC			LightDesc{};

    LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
    //LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);

    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vPosition = _float4(20.f, 3.f, 20.f, 1.f);
    LightDesc.fRange = 10.f;

    LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.4f, 0.2f, 0.2f, 1.f);
    LightDesc.vSpecular = _float4(1.f, 0.4f, 0.4f, 1.f);
    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

    LightDesc.eType = LIGHT_DESC::TYPE_POINT;
    LightDesc.vPosition = _float4(30.f, 3.f, 20.f, 1.f);
    LightDesc.fRange = 10.f;

    LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
    LightDesc.vAmbient = _float4(0.2f, 0.4f, 0.2f, 1.f);
    LightDesc.vSpecular = _float4(0.4f, 1.f, 0.4f, 1.f);
    if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
        return E_FAIL;

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
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_UI_Loading"), TEXT("Prototype_GameObject_UI_Loading"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_UI_Menu"), TEXT("Prototype_GameObject_UI_Menu"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_UI_Stat"), TEXT("Prototype_GameObject_UI_Stat"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_UI_LockOn"), TEXT("Prototype_GameObject_UI_LockOn"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_UI_Inventory"), TEXT("Prototype_GameObject_UI_Inventory"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_UI_Slot"), TEXT("Prototype_GameObject_UI_Slot"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_UI_Potion"), TEXT("Prototype_GameObject_UI_Potion"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_UI_Obtain"), TEXT("Prototype_GameObject_UI_Obtain"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Menu::Ready_Layer_Effect()
{
    // SPRITE
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Sprite_Sweat"), TEXT("Prototype_GameObject_Sprite_Sweat"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Menu::Ready_Layer_Player()
{
    // Desc
    CPlayer::PLAYER_DESC tDesc = {};
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Player";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    tDesc.strModelComTag = wstr;

    // Clone
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &tDesc)))
        return E_FAIL;

    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
    pPlayer->Set_Level(LEVEL_MENU);

    return S_OK;
}

HRESULT CLevel_Menu::Ready_Layer_Camera()
{
    // Camera_Free
    CCamera_Free::CAMERA_FREE_DESC tCameraFreeDesc{};
    tCameraFreeDesc.fMouseSensor = 0.5f;
    tCameraFreeDesc.fFovy = XMConvertToRadians(60.0f);
    tCameraFreeDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    tCameraFreeDesc.fNear = 0.1f;
    tCameraFreeDesc.fFar = 1000.0f;

    //tCameraFreeDesc.vEye = _float4(-75.f, 13.f, 58.f, 1.f);
    //tCameraFreeDesc.vAt = _float4(-75.f, 3.f, 68.f, 1.f);

    tCameraFreeDesc.vEye = { 65.f, 13.972946f, -75.f, 1.f };
    tCameraFreeDesc.vAt = { 65.f, 2.f, -62.f, 1.f };

    tCameraFreeDesc.fSpeedPerSec = 12.f;
    tCameraFreeDesc.fRotationPerSec = XMConvertToRadians(10.0f);

    CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Camera_Free"), &tCameraFreeDesc));
    if (nullptr == pCamera)
        return E_FAIL;

    m_pGameInstance->Add_Camera(TEXT("Camera_Free"), pCamera);

    // Camera_Follow
    CCamera_Follow::CAMERA_FOLLOW_DESC		tCameraFollowDesc{};
    tCameraFollowDesc.fFovy = XMConvertToRadians(60.0f);
    tCameraFollowDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    tCameraFollowDesc.fNear = 0.1f;
    tCameraFollowDesc.fFar = 1000.0f;

    tCameraFollowDesc.vEye = _float4(65.f, 15.f, -75.f, 1.f);
    tCameraFollowDesc.vAt = _float4(65.f, 2.f, -62.f, 1.f);

    //tCameraFollowDesc.vEye = _float4(0.f, 13.f, -13.f, 1.f);
    //tCameraFollowDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

    tCameraFollowDesc.fSpeedPerSec = 3.f;
    tCameraFollowDesc.fRotationPerSec = XMConvertToRadians(10.0f);

    pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Camera_Follow"), &tCameraFollowDesc));
    if (nullptr == pCamera)
        return E_FAIL;

    m_pGameInstance->Add_Camera(TEXT("Camera_Follow"), pCamera);

    // Camera_LockOn
    CCamera_LockOn::CAMERA_LOCKON_DESC		tCameraLockOnDesc{};
    tCameraLockOnDesc.fFovy = XMConvertToRadians(60.0f);
    tCameraLockOnDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    tCameraLockOnDesc.fNear = 0.1f;
    tCameraLockOnDesc.fFar = 1000.0f;
    tCameraLockOnDesc.vEye = _float4(0.f, 13.f, -13.f, 1.f);
    tCameraLockOnDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    tCameraLockOnDesc.fSpeedPerSec = 3.f;
    tCameraLockOnDesc.fRotationPerSec = XMConvertToRadians(10.0f);

    pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Camera_LockOn"), &tCameraLockOnDesc));
    if (nullptr == pCamera)
        return E_FAIL;

    m_pGameInstance->Add_Camera(TEXT("Camera_LockOn"), pCamera);

    // Camera_Puzzle
    CCamera::CAMERA_DESC		tCameraPuzzleDesc{};
    tCameraPuzzleDesc.fFovy = XMConvertToRadians(60.0f);
    tCameraPuzzleDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    tCameraPuzzleDesc.fNear = 0.1f;
    tCameraPuzzleDesc.fFar = 1000.0f;
    //tCameraPuzzleDesc.vEye = _float4(0.f, 13.f, -13.f, 1.f);
    //tCameraPuzzleDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

    tCameraPuzzleDesc.vEye = _float4(0.f, 0.02f, -61.f, 1.f);
    tCameraPuzzleDesc.vAt = _float4(0.f, 0.02f, -51.f, 1.f);

    tCameraPuzzleDesc.fSpeedPerSec = 3.f;
    tCameraPuzzleDesc.fRotationPerSec = XMConvertToRadians(10.0f);

    pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Camera_Puzzle"), &tCameraPuzzleDesc));
    if (nullptr == pCamera)
        return E_FAIL;

    m_pGameInstance->Add_Camera(TEXT("Camera_Puzzle"), pCamera);

    // Camera_Top
    CCamera::CAMERA_DESC		tCameraTopDesc{};
    tCameraTopDesc.fFovy = XMConvertToRadians(60.0f);
    tCameraTopDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    tCameraTopDesc.fNear = 0.1f;
    tCameraTopDesc.fFar = 1000.0f;

    tCameraTopDesc.vEye = _float4(0.f, 0.02f, -61.f, 1.f);
    tCameraTopDesc.vAt = _float4(0.f, 0.02f, -51.f, 1.f);

    tCameraTopDesc.fSpeedPerSec = 3.f;
    tCameraTopDesc.fRotationPerSec = XMConvertToRadians(10.0f);

    pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Camera_Top"), &tCameraTopDesc));
    if (nullptr == pCamera)
        return E_FAIL;

    m_pGameInstance->Add_Camera(TEXT("Camera_Top"), pCamera);

    // Camera_Telescope
    CCamera::CAMERA_DESC		tCameraTelescopeDesc{};
    tCameraTelescopeDesc.fFovy = XMConvertToRadians(60.0f);
    tCameraTelescopeDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    tCameraTelescopeDesc.fNear = 0.1f;
    tCameraTelescopeDesc.fFar = 1000.0f;

    tCameraTelescopeDesc.vEye = _float4(0.f, 0.02f, -61.f, 1.f);
    tCameraTelescopeDesc.vAt = _float4(0.f, 0.02f, -51.f, 1.f);

    tCameraTelescopeDesc.fSpeedPerSec = 3.f;
    tCameraTelescopeDesc.fRotationPerSec = XMConvertToRadians(10.0f);

    pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Camera_Telescope"), &tCameraTelescopeDesc));
    if (nullptr == pCamera)
        return E_FAIL;

    m_pGameInstance->Add_Camera(TEXT("Camera_Telescope"), pCamera);

    // Camera_Scene
    CCamera::CAMERA_DESC		tCameraSceneDesc{};
    tCameraSceneDesc.fFovy = XMConvertToRadians(60.0f);
    tCameraSceneDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    tCameraSceneDesc.fNear = 0.1f;
    tCameraSceneDesc.fFar = 1000.0f;

    tCameraSceneDesc.vEye = _float4(0.f, 0.02f, -61.f, 1.f);
    tCameraSceneDesc.vAt = _float4(0.f, 0.02f, -51.f, 1.f);

    tCameraSceneDesc.fSpeedPerSec = 3.f;
    tCameraSceneDesc.fRotationPerSec = XMConvertToRadians(10.0f);

    pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Camera_Scene"), &tCameraSceneDesc));
    if (nullptr == pCamera)
        return E_FAIL;

    m_pGameInstance->Add_Camera(TEXT("Camera_Scene"), pCamera);

    m_pGameInstance->Change_Camera(TEXT("Camera_Free"));

    m_pGameInstance->Set_Camera_Level(LEVEL_MENU);

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

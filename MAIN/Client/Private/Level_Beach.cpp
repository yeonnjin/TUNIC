#include "stdafx.h"
#include "Level_Beach.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Player.h"
#include "Monster.h"
#include "Map.h"
#include "Sky.h"

#include "Object_Chest.h"

#include "Camera.h"
#include "Camera_Free.h"
#include "Camera_Follow.h"
#include "Camera_LockOn.h"

#include "UI_Loading.h"

CLevel_Beach::CLevel_Beach(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Beach::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Ready_Lights()))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI()))
        return E_FAIL;

    if (FAILED(Ready_LandObject()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera()))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, TEXT("Layer_Editor"), TEXT("Prototype_GameObject_Editor"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Beach::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (true == m_pGameInstance->Get_DIKeyState(DIK_RCONTROL, KEY_DOWN))
    {
        CUI_Loading* pUILoading = dynamic_cast<CUI_Loading*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_UI_Loading")));
        pUILoading->Set_Using(true);

        if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_SHOP))))
            return;
    }
}

HRESULT CLevel_Beach::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    SetWindowText(g_hWnd, TEXT("LEVEL : BEACH"));

    return S_OK;
}

HRESULT CLevel_Beach::Ready_Lights()
{
    LIGHT_DESC			LightDesc{};

    LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
    LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

    LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
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

HRESULT CLevel_Beach::Ready_LandObject()
{
    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Beach::Ready_Layer_Player(const wstring& strLayerTag)
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
    pPlayer->Set_Level(LEVEL_BEACH);
    if (FAILED(pPlayer->Set_Navigation(LEVEL_BEACH)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Beach::Ready_Layer_Monster(const wstring& strLayerTag)
{
    // BLOB
    CMonster::Monster_Desc tDesc = {};
    _char szModelTag2[MAX_PATH] = "Prototype_Component_Model_Monster_Blob_Normal";
    wstring wstr2(&szModelTag2[0], &szModelTag2[MAX_PATH]);
    tDesc.strModelComTag = wstr2;
    tDesc.eLevel = LEVEL_BEACH;
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, strLayerTag, TEXT("Prototype_GameObject_Monster_Blob"), &tDesc)))
        return E_FAIL;

	// SPINNER
    tDesc = {};
	_char szModelTag[MAX_PATH] = "Prototype_Component_Model_Monster_Spinner";
	wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
	tDesc.strModelComTag = wstr;
    tDesc.eLevel = LEVEL_BEACH;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, strLayerTag, TEXT("Prototype_GameObject_Monster_Spinner"), &tDesc)))
		return E_FAIL;

    // COWBOT
	tDesc = {};
	_char szModelTag3[MAX_PATH] = "Prototype_Component_Model_Monster_CowBot";
	wstring wstr3(&szModelTag3[0], &szModelTag3[MAX_PATH]);
	tDesc.strModelComTag = wstr3;	
    tDesc.eLevel = LEVEL_BEACH;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, strLayerTag, TEXT("Prototype_GameObject_Monster_CowBot"), &tDesc)))
		return E_FAIL;

    // GUARD
	tDesc = {};
	_char szModelTag4[MAX_PATH] = "Prototype_Component_Model_Monster_Guard";
	wstring wstr4(&szModelTag4[0], &szModelTag4[MAX_PATH]);
	tDesc.strModelComTag = wstr4;
    tDesc.eLevel = LEVEL_BEACH;
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, strLayerTag, TEXT("Prototype_GameObject_Monster_Guard"), &tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Beach::Ready_Layer_Map(const wstring& strLayerTag)
{
    CMap::MAP_DESC tDesc = {};
    tDesc.isRotation = true;
    tDesc.vPosition = _float3(0.f, 0.f, 0.f);
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Map_Beach";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    tDesc.strModelComTag = wstr;
    tDesc.eLevel = LEVEL_BEACH;
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, strLayerTag, TEXT("Prototype_GameObject_Map"), &tDesc)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, strLayerTag, TEXT("Prototype_GameObject_Map_WaterEdge"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, strLayerTag, TEXT("Prototype_GameObject_Map_WaterBlue"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, strLayerTag, TEXT("Prototype_GameObject_Map_WaterDepth"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Beach::Ready_Layer_BackGround(const wstring& strLayerTag)
{
    CSky::SKY_DESC tDesc{};
    tDesc.strSkyComTag = TEXT("Prototype_Component_Texture_Sky");

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, strLayerTag, TEXT("Prototype_GameObject_Sky"), &tDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Beach::Ready_Layer_UI()
{
    return S_OK;
}

HRESULT CLevel_Beach::Ready_Layer_Object(const wstring& strLayerTag)
{
    // CHEST
    CObject_Chest::CHEST_DESC tDesc = {};
    tDesc.vPosition = _vector{ 76.f, 2.5f, -65.f, 1.f };
    tDesc.eType = CItem::TYPE_WEAPON;
    tDesc.eItem = CItem::ITEM_STICK;
    tDesc.isRotation = true;
    tDesc.fAngle = 90.f;
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, strLayerTag, TEXT("Prototype_GameObject_Object_Chest"), &tDesc)))
        return E_FAIL;

    tDesc = {};
    tDesc.vPosition = _vector{ 74.f, 4.f, -98.f, 1.f };
    tDesc.eType = CItem::TYPE_UTILE;
    tDesc.eItem = CItem::ITEM_SHIELD;
    tDesc.isRotation = true;
    tDesc.fAngle = 90.f;
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, strLayerTag, TEXT("Prototype_GameObject_Object_Chest"), &tDesc)))
        return E_FAIL;

    tDesc = {};
    tDesc.vPosition = _vector{ 28.f, 2.1f, -69.f, 1.f };
    tDesc.eType = CItem::TYPE_WEAPON;
    tDesc.eItem = CItem::ITEM_SWORD;
    tDesc.isRotation = true;
    tDesc.fAngle = -90.f;
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, strLayerTag, TEXT("Prototype_GameObject_Object_Chest"), &tDesc)))
        return E_FAIL;

    tDesc = {};
    tDesc.vPosition = _vector{ 4.2f, 1.5f, -72.f, 1.f };
    tDesc.eType = CItem::TYPE_WEAPON;
    tDesc.eItem = CItem::ITEM_WAND;
    tDesc.isRotation = true;
    tDesc.fAngle = 90.f;
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, strLayerTag, TEXT("Prototype_GameObject_Object_Chest"), &tDesc)))
        return E_FAIL;

    // TELESCOPE
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, TEXT("Layer_Object_Telescope"), TEXT("Prototype_GameObject_Object_Telescope"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Beach::Ready_Layer_Camera()
{
    CCamera_Follow* pCamera = dynamic_cast<CCamera_Follow*>(m_pGameInstance->Get_Camera(TEXT("Camera_Follow")));
    CTransform* pCameraTransform = dynamic_cast<CTransform*>(pCamera->Get_Component(g_strTransformTag));

    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
    CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag));
    _vector vPlayerPosition = pCameraTransform->Get_State_Vector(CTransform::STATE_POSITION);

    //_vector vCamPosition = { vPlayerPosition.m128_f32[0], vPlayerPosition.m128_f32[1] + 13.f, vPlayerPosition.m128_f32[2] - 13.f, 1.f };
    _vector vCamPosition = { 65.f, 13.972946f, -75.f, 1.f };
    pCameraTransform->Set_State(CTransform::STATE_POSITION, vCamPosition);
    pCameraTransform->Look_At(vPlayerPosition);

    m_pGameInstance->Change_Camera(TEXT("Camera_Follow"));
    m_pGameInstance->Set_Camera_Level(LEVEL_BEACH);

    return S_OK;
}

CLevel_Beach* CLevel_Beach::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Beach* pInstance = new CLevel_Beach(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Create : CLevel_Beach"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Beach::Free()
{
    __super::Free();
}

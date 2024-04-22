#include "stdafx.h"
#include "Level_Tool_Map.h"

#include "Editor.h"
#include "Camera_Free.h"

#include "Map.h"
#include "Map_Object.h"
#include "Test_Object.h"

CLevel_Tool_Map::CLevel_Tool_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Tool_Map::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Ready_Layer_Editor(TEXT("Layer_Editor"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
        return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool_Map::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("LEVEL : TOOL_MAP"));

	return S_OK;
}

HRESULT CLevel_Tool_Map::Ready_Layer_Editor(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, strLayerTag, TEXT("Prototype_GameObject_Editor"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool_Map::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC		CameraDesc{};

	CameraDesc.fMouseSensor = 0.1f;
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.0f;
	CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, strLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool_Map::Ready_Layer_Terrain(const wstring& strLayerTag)
{
	/*if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_Tool_Map::Ready_Layer_Object(const wstring& strLayerTag)
{
	/*if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT("Layer_Map_Object"), TEXT("Prototype_GameObject_Map"))))
		return E_FAIL;*/

	// MAP OBJECT ============================================================================================================================================

	// Player ================================================================================================================================================
	/*CTest_Object::TEST_DESC tDesc = {};
	tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	tDesc.strModelComTag = TEXT("Prototype_Component_Model_Player");
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Test_Object"), &tDesc)))
		return E_FAIL;*/

	//CMap_Object::MAPOBJ_DESC tObjDesc = {};
	//tObjDesc.isLoad = false;
	//tObjDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tObjDesc.strModelComTag = TEXT("Prototype_Component_Model_Weapon_Wandbow");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tObjDesc)))
	//	return E_FAIL;

	//CMap_Object::MAPOBJ_DESC tObjDesc = {};
	//tObjDesc.isLoad = false;
	//tObjDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tObjDesc.strModelComTag = TEXT("Prototype_Component_Model_Weapon_Wandbow_Beam");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tObjDesc)))
	//	return E_FAIL;
	

	// Boss ==================================================================================================================================================
	//CTest_Object::TEST_DESC tDesc = {};
 //
	//tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tDesc.strModelComTag =  TEXT("Prototype_Component_Model_Boss_Librarian");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Test_Object"), &tDesc)))
	//	return E_FAIL;

	//CMap_Object::MAPOBJ_DESC tObjDesc = {};
	//tObjDesc.isLoad = false;
	//tObjDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tObjDesc.strModelComTag = TEXT("Prototype_Component_Model_Boss_Librarian_Effect_Orb");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tObjDesc)))
	//	return E_FAIL;

	//CTest_Object::TEST_DESC tDesc = {};
	//tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tDesc.strModelComTag =  TEXT("Prototype_Component_Model_Boss_Librarian_Effect_Beam");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Test_Object"), &tDesc)))
	//	return E_FAIL;

	//CMap_Object::MAPOBJ_DESC tObjDesc = {};
	//tObjDesc.isLoad = false;
	//tObjDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tObjDesc.strModelComTag = TEXT("Prototype_Component_Model_Boss_Librarian_Effect_Slash_Horizon");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tObjDesc)))
	//	return E_FAIL;

	//CMap_Object::MAPOBJ_DESC tObjDesc = {};
	//tObjDesc.isLoad = false;
	//tObjDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tObjDesc.strModelComTag = TEXT("Prototype_Component_Model_Boss_Librarian_Sword");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tObjDesc)))
	//	return E_FAIL;

	// ========================================================================================================================================================

	//tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tDesc.strModelComTag =  TEXT("Prototype_Component_Model_Monster_Bat");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Test_Object"), &tDesc)))
	//	return E_FAIL;

	//tDesc = {};
	//tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tDesc.strModelComTag = TEXT("Prototype_Component_Model_Monster_Blob");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Monster"), TEXT("Prototype_GameObject_Test_Object"), &tDesc)))
	//	return E_FAIL;

	//tDesc = {};
	//tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tDesc.strModelComTag = TEXT("Prototype_Component_Model_Monster_Blob_Normal");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Test_Object"), &tDesc)))
	//	return E_FAIL;

	//tDesc = {};
	//tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tDesc.strModelComTag = TEXT("Prototype_Component_Model_Monster_CowBot");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Test_Object"), &tDesc)))
	//	return E_FAIL;

	//CMap_Object::MAPOBJ_DESC tDesc = {};
	//tDesc.isLoad = false;
	//tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tDesc.strModelComTag = TEXT("Prototype_Component_Model_Monster_CowBot_Shield");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tDesc)))
	//	return E_FAIL;

	//tDesc = {};
	//tDesc.isLoad = false;
	//tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tDesc.strModelComTag = TEXT("Prototype_Component_Model_Monster_CowBot_Sword");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tDesc)))
	//	return E_FAIL;

	//tDesc = {};
	//tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tDesc.strModelComTag = TEXT("Prototype_Component_Model_Monster_Spinner");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Test_Object"), &tDesc)))
	//	return E_FAIL;

	//tDesc = {};
	//tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tDesc.strModelComTag = TEXT("Prototype_Component_Model_Monster_Crab");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Test_Object"), &tDesc)))
	//	return E_FAIL;

	//tDesc = {};
	//tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tDesc.strModelComTag = TEXT("Prototype_Component_Model_Monster_Guard");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Test_Object"), &tDesc)))
	//	return E_FAIL;

	//CMap_Object::MAPOBJ_DESC tObjDesc = {};
	//tObjDesc.isLoad = false;
	//tObjDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tObjDesc.strModelComTag = TEXT("Prototype_Component_Model_Monster_Guard_Shield");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tObjDesc)))
	//	return E_FAIL;

	//tObjDesc = {};
	//tObjDesc.isLoad = false;
	//tObjDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tObjDesc.strModelComTag = TEXT("Prototype_Component_Model_Monster_Guard_Spear");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tObjDesc)))
	//	return E_FAIL;

	//tDesc = {};
	//tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tDesc.strModelComTag = TEXT("Prototype_Component_Model_Monster_Frog");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Test_Object"), &tDesc)))
	//	return E_FAIL;

	//CMap_Object::MAPOBJ_DESC tObjDesc = {};
	//tObjDesc.isLoad = false;
	//tObjDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tObjDesc.strModelComTag = TEXT("Prototype_Component_Model_Monster_Frog_Scimitar");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tObjDesc)))
	//	return E_FAIL;

	//CTest_Object::TEST_DESC tObjDesc = {};

 //   tObjDesc.vPosition = _float3(0.f, 18.f, 56.f);
 //   tObjDesc.strModelComTag =  TEXT("Prototype_Component_Model_NPC_Merchant");
 //   if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Test_Object"), &tObjDesc)))
 //   	return E_FAIL;

	// MAP ===================================================================================================================================================
	CMap::MAP_DESC tMapDesc = {};
	tMapDesc.isLoad = false;
	tMapDesc.vPosition = _float3(0.f, 0.f, 0.f);
	// Prototype_Component_Model_Map_Beach
	// Prototype_Component_Model_Map_FOXGOD
	// Prototype_Component_Model_Map_Librarian
	// Prototype_Component_Model_Map_Shop
	tMapDesc.strModelComTag = TEXT("Prototype_Component_Model_Map_FOXGOD");
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map"), TEXT("Prototype_GameObject_Map"), &tMapDesc)))
		return E_FAIL;	


	// OBJECT ===================================================================================================================================================
	//CTest_Object::TEST_DESC tDesc = {};
	//tDesc.strModelComTag = TEXT("Prototype_Component_Model_Object_Chest");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Test_Object"), &tDesc)))
	//	return E_FAIL;

	//CMap_Object::MAPOBJ_DESC tObjDesc = {};
	//tObjDesc.isLoad = false;
	//tObjDesc.vPosition = _float3(0.f, 0.f, 0.f);
	//tObjDesc.strModelComTag = TEXT("Prototype_Component_Model_Item_Berry_HP");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tObjDesc)))
	//	return E_FAIL;

	//tObjDesc = {};
	//tObjDesc.isLoad = false;
	//tObjDesc.vPosition = _float3(1.f, 0.f, 0.f);
	//tObjDesc.strModelComTag = TEXT("Prototype_Component_Model_Item_Berry_MP");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tObjDesc)))
	//	return E_FAIL;

	//tObjDesc = {};
	//tObjDesc.isLoad = false;
	//tObjDesc.vPosition = _float3(2.f, 0.f, 0.f);
	//tObjDesc.strModelComTag = TEXT("Prototype_Component_Model_Item_IceBomb");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tObjDesc)))
	//	return E_FAIL;

	//tObjDesc = {};
	//tObjDesc.isLoad = false;
	//tObjDesc.vPosition = _float3(3.f, 0.f, 0.f);
	//tObjDesc.strModelComTag = TEXT("Prototype_Component_Model_Item_Laurel");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tObjDesc)))
	//	return E_FAIL;

	//tObjDesc = {};
	//tObjDesc.isLoad = false;
	//tObjDesc.vPosition = _float3(4.f, 0.f, 0.f);
	//tObjDesc.strModelComTag = TEXT("Prototype_Component_Model_Item_Potion");
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tObjDesc)))
	//	return E_FAIL;

	CMap_Object::MAPOBJ_DESC tObjDesc = {};
	tObjDesc.isLoad = false;
	tObjDesc.vPosition = _float3(4.f, 0.f, 0.f);
	tObjDesc.strModelComTag = TEXT("Prototype_Component_Model_Object_Telescope");
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT(/*"Layer_Map_Object"*/"Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tObjDesc)))
		return E_FAIL;


	// WEAPON =================================================================================================================================================== 

	/*if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Test_Object"), TEXT("Prototype_GameObject_Test_Object"))))
		return E_FAIL;*/

	/*CMap::MAP_DESC tDesc = {}; */
	/*tDesc.isLoad = false;
	tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	tDesc.strModelComTag = TEXT("Prototype_Component_Model_Weapon_Sword");
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT("Layer_Map_Object"), TEXT("Prototype_Component_Model_Weapon_Sword"), &tDesc)))
		return E_FAIL;*/

	/*if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, strLayerTag, TEXT("Prototype_GameObject_Test_Object"))))
		return E_FAIL;*/

	/*for (size_t i = 0; i < 5; ++i)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, strLayerTag, TEXT("Prototype_GameObject_Map_Object"))))
			return E_FAIL;
	}*/
	

	return S_OK;
}

CLevel_Tool_Map* CLevel_Tool_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool_Map* pInstance = new CLevel_Tool_Map(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Create : CLevel_Tool_Map"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tool_Map::Free()
{
	__super::Free();
}

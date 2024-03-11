#include "stdafx.h"
#include "Level_Tool_Map.h"

#include "Editor.h"
#include "Camera_Free.h"
#include "Map_Object.h"

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

	CMap_Object::MAPOBJ_DESC tDesc = {};
	tDesc.isLoad = false;
	tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	tDesc.strModelComTag = TEXT("Prototype_Component_Model_Map");
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT("Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tDesc)))
		return E_FAIL;

	/*if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, strLayerTag, TEXT("Prototype_GameObject_Test_Object"))))
		return E_FAIL;*/

	/*for (size_t i = 0; i < 5; ++i)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, strLayerTag, TEXT("Prototype_GameObject_Map_Object"))))
			return E_FAIL;
	}*/
	

	return S_OK;

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

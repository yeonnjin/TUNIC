#include "stdafx.h"
#include "Level_GamePlay.h"

#include "Camera_Free.h"
#include "Map_Object.h"
#include "Player.h"

#include "Editor.h"

#include <fstream>

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Editor(TEXT("Layer_Editor"))))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if(FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_LandObject()))
		return E_FAIL;

	if(FAILED(Ready_Layer_MapObj(TEXT("Layer_MapObject"))))
		return E_FAIL;

	/*
	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;*/

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("LEVEL : GAMEPLAY"));

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_Editor(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Editor"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC	tLightDesc{};

	tLightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	tLightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	tLightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	tLightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	tLightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(tLightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring & strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC		CameraDesc{};

	CameraDesc.fMouseSensor = 0.1f;
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.0f;
	CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fSpeedPerSec = 5.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObject()
{
	///* 랜드오브젝트용 객체들에게 필요한 데이터를 구한다.*/
	//CLandObject::LANDOBJECT_DESC		LandObjectDesc = {};
	//LandObjectDesc.pTerrainTransform = (CTransform*)(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), g_strTransformTag));
	//LandObjectDesc.pTerrainVIBuffer = (CVIBuffer_Terrain*)(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));	

	///* 구한정보들을 각 랜드오브젝트르 생성할 때 던진다. */
	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	/* 구한정보들을 각 랜드오브젝트르 생성할 때 던진다. */
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring & strLayerTag)
{
	// Desc
	CPlayer::PLAYER_DESC tDesc = {};
	_char szModelTag[MAX_PATH] = "Prototype_Component_Model_Player";
	wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
	tDesc.strModelComTag = wstr;

	// Clone
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &tDesc)))
		return E_FAIL;

	/*if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &tDesc)))
		return E_FAIL;*/

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag)
{
	/*if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring & strLayerTag)
{
	/*for (size_t i = 0; i < 20; i++)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Effect"))))
			return E_FAIL;
	}
*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_MapObj(const wstring& strLayerTag)
{
	////Desc
	//CMap_Object::MAPOBJ_DESC tDesc = {};
	//_char szModelTag[MAX_PATH] = "Prototype_Component_Model_Map_Object";
	//wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
	//tDesc.strModelComTag = wstr;
	//_vector vScale = { 1.f, 1.f, 1.f };
	//_vector vRoation = { 0.f, 0.f, 0.f, 1.f };
	//_vector vTanslation = { 1.f, 1.f, 1.f, 1.f };
	//_vector vAxiz = { 0.f, 1.f, 0.f };

	//XMStoreFloat4x4(&tDesc.TransformMatrix, XMMatrixAffineTransformation(vScale, vAxiz, vRoation, vTanslation));

	//// Clone
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Map_Object"), TEXT("Prototype_GameObject_Map_Object"), &tDesc)))
	//	return E_FAIL;

	//ifstream fin;
	//fin.open("../Bin/Resources/Data/Stick.dat", ios::in | ios::binary);

	//// Object Count
	//_uint iNumObjects;
	//fin.read(reinterpret_cast<char*>(&iNumObjects), sizeof(_uint));
	//for (size_t i = 0; i < iNumObjects; ++i)
	//{
	//	MAPOBJFILE tMapObjFile = {};

	//	// TransformMatrix
	//	fin.read(reinterpret_cast<char*>(&tMapObjFile.TransformMatrix), sizeof(_float4x4));

	//	// ModelComTag
	//	fin.read(reinterpret_cast<char*>(&tMapObjFile.szModelComTag), sizeof(_char) * MAX_PATH);

	//	// Desc
	//	CMap_Object::MAPOBJ_DESC tDesc = {};
	//	tDesc.TransformMatrix = tMapObjFile.TransformMatrix;

	//	wstring wstr(&tMapObjFile.szModelComTag[0], &tMapObjFile.szModelComTag[MAX_PATH]);
	//	tDesc.strModelComTag = wstr;

	//	// Clone
	//	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Map_Object"), &tDesc)))
	//		return E_FAIL;
	//}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	/*if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;*/

	/*
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;
	*/
	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Create : CLevel_GamePlay"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}

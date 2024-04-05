#include "stdafx.h"
#include "Level_GamePlay.h"

#include "Camera_Free.h"
#include "Camera_Follow.h"
#include "Camera_LockOn.h"

#include "Map_Object.h"
#include "Player.h"

#include "Monster.h"

#include "Editor.h"
#include "Map.h"
#include "Cell.h"

#include <fstream>

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_LandObject()))
		return E_FAIL;

	if(FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Editor(TEXT("Layer_Editor"))))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (0 == m_pGameInstance->Get_Object_Count(LEVEL_GAMEPLAY, TEXT("Layer_Monster")))
	{
		CMonster::Monster_Desc tDesc = {};
		_char szModelTag[MAX_PATH] = "Prototype_Component_Model_Monster_Spinner";
		wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
		tDesc.strModelComTag = wstr;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster_Spinner"), &tDesc)))
			return;

		tDesc = {};
		_char szModelTag1[MAX_PATH] = "Prototype_Component_Model_Monster_Bat";
		wstring wstr1(&szModelTag1[0], &szModelTag1[MAX_PATH]);
		tDesc.strModelComTag = wstr1;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster_Bat"), &tDesc)))
			return;
	}

	if (true == m_pGameInstance->Get_DIKeyState(DIK_NUMPAD0, KEY_DOWN))
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Red"))))
			return;
	}
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
	// 1. 로딩부분에서 카메라 다 넣어주기
	// 2. 각 레벨 마다 카메라 매니저에 레벨 세팅해주기

	// Camera_Free
	CCamera_Free::CAMERA_FREE_DESC tCameraFreeDesc{};
	tCameraFreeDesc.fMouseSensor = 1.f;
	tCameraFreeDesc.fFovy = XMConvertToRadians(60.0f);
	tCameraFreeDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	tCameraFreeDesc.fNear = 0.1f;
	tCameraFreeDesc.fFar = 1000.0f;
	tCameraFreeDesc.vEye = _float4(0.f, 13.f, -13.f, 1.f);
	tCameraFreeDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	tCameraFreeDesc.fSpeedPerSec = 3.f;
	tCameraFreeDesc.fRotationPerSec = XMConvertToRadians(90.0f);

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
	tCameraFollowDesc.vEye = _float4(0.f, 13.f, -13.f, 1.f);
	tCameraFollowDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
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


	m_pGameInstance->Change_Camera(TEXT("Camera_Free"));

	m_pGameInstance->Set_Camera_Level(LEVEL_GAMEPLAY);

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
	//// Desc
	//CPlayer::PLAYER_DESC tDesc = {};
	//_char szModelTag[MAX_PATH] = "Prototype_Component_Model_Player";
	//wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
	//tDesc.strModelComTag = wstr;

	//// Clone
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &tDesc)))
	//	return E_FAIL;

	// Desc
	CPlayer::PLAYER_DESC tDesc = {};
	_char szModelTag[MAX_PATH] = "Prototype_Component_Model_Player";
	wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
	tDesc.strModelComTag = wstr;

	// Clone
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &tDesc)))
		return E_FAIL;

	dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")))->Set_Level(LEVEL_GAMEPLAY);

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag)
{
	// Desc
	CMonster::Monster_Desc tDesc = {};
	_char szModelTag[MAX_PATH] = "Prototype_Component_Model_Monster_Spinner";
	wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
	tDesc.strModelComTag = wstr;

	for (size_t i = 0; i < 2; ++i)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster_Spinner"), &tDesc)))
			return E_FAIL;
	}
	

	// Desc
	/*CMonster::Monster_Desc*/ tDesc = {};
	_char szModelTag1[MAX_PATH] = "Prototype_Component_Model_Monster_Bat";
	wstring wstr1(&szModelTag1[0], &szModelTag1[MAX_PATH]);
	tDesc.strModelComTag = wstr1;

	for (size_t i = 0; i < 2; ++i)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster_Bat"), &tDesc)))
			return E_FAIL;
	}

	tDesc = {};
	_char szModelTag2[MAX_PATH] = "Prototype_Component_Model_Monster_Blob_Normal";
	wstring wstr2(&szModelTag2[0], &szModelTag2[MAX_PATH]);
	tDesc.strModelComTag = wstr2;

	for (size_t i = 0; i < 3; ++i)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster_Blob"), &tDesc)))
			return E_FAIL;
	}

	tDesc = {};
	_char szModelTag3[MAX_PATH] = "Prototype_Component_Model_Monster_CowBot";
	wstring wstr3(&szModelTag3[0], &szModelTag3[MAX_PATH]);
	tDesc.strModelComTag = wstr3;

	for (size_t i = 0; i < 1; ++i)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster_CowBot"), &tDesc)))
			return E_FAIL;
	}
	

	// "Prototype_Component_Model_Monster_Bat"
	// "Prototype_Component_Model_Monster_Blob"
	// "Prototype_Component_Model_Monster_Blob_Normal"
	// "Prototype_Component_Model_Monster_CowBot"
	// "Prototype_Component_Model_Monster_CowBot_Shield"
	// "Prototype_Component_Model_Monster_CowBot_Sword"
	// "Prototype_Component_Model_Monster_Spinner"

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring & strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Particle_Blue"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Map(const wstring& strLayerTag)
{
	// TODO:맵 생성시 네비 태그 같이 전달
	CMap::MAP_DESC tDesc = {};
	tDesc.isLoad = false;
	tDesc.vPosition = _float3(0.f, 0.f, 0.f);
	_char szModelTag[MAX_PATH] = /*"Prototype_Component_Model_Map_Beach";*/"Prototype_Component_Model_Map_FOXGOD";/*"Prototype_Component_Model_Map_Librarian";*/
	wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
	tDesc.strModelComTag = wstr;
	//tDesc.strModelComTag = TEXT("Prototype_Component_Model_Map_FOXGOD");
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Map"), &tDesc)))
		return E_FAIL;

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

	
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;
	
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

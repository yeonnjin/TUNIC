#include "stdafx.h"
#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Monster_Spinner.h"

#include "Player.h"

#include "Camera.h"
#include "Camera_Free.h"
#include "Camera_Follow.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);	

	/*m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, 0x);*/
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC		EngineDesc = {};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.isWindowed = true;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;

	/* 내 게임의 기초 초기화 과정 */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Ready_Fonts()))
		return E_FAIL;
#endif // _DEBUG

	if (FAILED(Open_Level(LEVEL_GAMEPLAY)))
		return E_FAIL;
	
	if (FAILED(Ready_Player()))
		return E_FAIL;

	/*if (FAILED(Ready_Camera()))
		return E_FAIL;*/

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	// Engine : Tick
	m_pGameInstance->Tick_Engine(fTimeDelta);

	// Collision
	m_pGameInstance->Check_Collision_Groups(CCollision_Manager::GROUP_PLAYER_WEAPON, CCollision_Manager::GROUP_MONSTER);
	m_pGameInstance->Check_Collision_Groups(CCollision_Manager::GROUP_PLAYER, CCollision_Manager::GROUP_MONSTER);
	m_pGameInstance->Check_Collision_Groups(CCollision_Manager::GROUP_PLAYER, CCollision_Manager::GROUP_MONSTER_WEAPON);
	m_pGameInstance->Check_Collision_Groups(CCollision_Manager::GROUP_PLAYER, CCollision_Manager::GROUP_INTERACTIVE);
	m_pGameInstance->Check_Collision_Groups(CCollision_Manager::GROUP_MONSTER, CCollision_Manager::GROUP_MONSTER);

	// Rigid
	m_pGameInstance->Check_Rigid_Groups();

	// Camera
	if (true == m_pGameInstance->Get_DIKeyState(DIK_G, KEY_DOWN))
		m_pGameInstance->Change_Camera(TEXT("Camera_Free"));
	else if (true == m_pGameInstance->Get_DIKeyState(DIK_H, KEY_DOWN))
		m_pGameInstance->Change_Camera(TEXT("Camera_Follow"));
	else if (true == m_pGameInstance->Get_DIKeyState(DIK_F, KEY_DOWN))
		m_pGameInstance->Change_Camera(TEXT("Camera_Puzzle"));
	else if (true == m_pGameInstance->Get_DIKeyState(DIK_T, KEY_DOWN))
		m_pGameInstance->Change_Camera(TEXT("Camera_Top"));

	// Engine : Late Tick
	m_pGameInstance->Late_Tick_Engine(fTimeDelta);	
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

#ifdef _DEBUG
	wsprintf(m_szFont, TEXT("튜닉"));
#endif // _DEBUG

	m_pGameInstance->Begin_Draw(_float4(0.f, 0.f, 1.f, 1.f));

	m_pGameInstance->Draw();	

#ifdef _DEBUG
	m_pGameInstance->Render_Font(TEXT("Font_Default"), m_szFont, _float2(0.f, 0.f), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);
#endif // _DEBUG

	m_pGameInstance->End_Draw();

	return S_OK;
}

HRESULT CMainApp::Ready_Fonts()
{
	// MakeSpriteFont "HY궁서B" /FontSize:30 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 140.spritefont
	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/140.spritefont"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (LEVEL_LOADING == eLevelID)
		return E_FAIL;

	m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID));

	return	S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{	
	// VIBuffer ===========================================================================================
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Shader =============================================================================================
	/* For.Prototype_Component_Shader_VtxPosTex */
 	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	// Texture ============================================================================================
	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* Prototype_Component_Texture_Sky_Shop */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky_Shop"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Sky/Shop.dds")))))
		return E_FAIL;

	return	S_OK;
}

HRESULT CMainApp::Ready_Player()
{
	//// Desc
	//CPlayer::PLAYER_DESC tDesc = {};
	//_char szModelTag[MAX_PATH] = "Prototype_Component_Model_Player";
	//wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
	//tDesc.strModelComTag = wstr;

	//// Clone
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &tDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Camera()
{
	//// Camera_Free
	//CCamera_Free::CAMERA_FREE_DESC tCameraFreeDesc{};
	//tCameraFreeDesc.fMouseSensor = 1.f;
	//tCameraFreeDesc.fFovy = XMConvertToRadians(60.0f);
	//tCameraFreeDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	//tCameraFreeDesc.fNear = 0.1f;
	//tCameraFreeDesc.fFar = 1000.0f;
	//tCameraFreeDesc.vEye = _float4(0.f, 13.f, -13.f, 1.f);
	//tCameraFreeDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	//tCameraFreeDesc.fSpeedPerSec = 3.f;
	//tCameraFreeDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	//CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Camera_Free"), &tCameraFreeDesc));
	//if (nullptr == pCamera)
	//	return E_FAIL;

	//m_pGameInstance->Add_Camera(TEXT("Camera_Free"), pCamera);

	//// Camera_Follow
	//CCamera_Follow::CAMERA_FOLLOW_DESC		tCameraFollowDesc{};
	//tCameraFollowDesc.pTargetTransform = (CTransform*)(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), g_strTransformTag, 0));
	//tCameraFollowDesc.fFovy = XMConvertToRadians(60.0f);
	//tCameraFollowDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	//tCameraFollowDesc.fNear = 0.1f;
	//tCameraFollowDesc.fFar = 1000.0f;
	//tCameraFollowDesc.vEye = _float4(0.f, 13.f, -13.f, 1.f);
	//tCameraFollowDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	//tCameraFollowDesc.fSpeedPerSec = 3.f;
	//tCameraFollowDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	//pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Camera_Follow"), &tCameraFollowDesc));
	//if (nullptr == pCamera)
	//	return E_FAIL;

	//m_pGameInstance->Add_Camera(TEXT("Camera_Follow"), pCamera);

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Create : CMainApp"));
		
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}

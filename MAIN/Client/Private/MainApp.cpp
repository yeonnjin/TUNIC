#include "stdafx.h"
#include "MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Monster_Spinner.h"

#include "Player.h"

#include "Camera.h"
#include "Camera_Free.h"
#include "Camera_Follow.h"
#include "Camera_LockOn.h"

#include <tchar.h>

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

	if (FAILED(Open_Level(LEVEL_MENU)))
		return E_FAIL;

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
	wsprintf(m_szFont, TEXT("TUNIC"));
#endif // _DEBUG

	m_pGameInstance->Begin_Draw(_float4(0.f, 0.f, 1.f, 1.f));

	m_pGameInstance->Draw();	

#ifdef _DEBUG
	m_pGameInstance->Render_Font(TEXT("Font_Odin_30"), m_szFont, _float2(0.f, 0.f), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);
#endif // _DEBUG

	m_pGameInstance->End_Draw();

	return S_OK;
}

HRESULT CMainApp::Ready_Fonts()
{
	// MakeSpriteFont "Odin Rounded" /FontSize:28 /FastPack Odin_28.spritefont
	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Odin_30"), TEXT("../Bin/Resources/Fonts/Odin_30.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Odin_35"), TEXT("../Bin/Resources/Fonts/Odin_35.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Odin_25"), TEXT("../Bin/Resources/Fonts/Odin_25.spritefont"))))
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

	/* For.Prototype_Component_Shader_VtxPosNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosNorTex.hlsl"), VTXPOSNORTEX::Elements, VTXPOSNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMeshMap"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshMap.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	// Collider ============================================================================================
	/* Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* Prototype_Component_Collider_SPHERE */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
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

	/* Prototype_Component_Texture_Sky_Boss */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky_Boss"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Sky/Boss.dds")))))
		return E_FAIL;

	// UI TEXTURE ==================================================================================================
	
	/* Prototype_Component_Texture_UI_Loading */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/Loading%d.png")))))
		return E_FAIL;
	
	/* Prototype_Component_Texture_UI_Stat */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Stat"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/STAT/STAT%d.png"), 13))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Inventory */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Inventory"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Inven%d.png"), 6))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Item */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Item"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Item/Item%d.png"), 13))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Item_Obtain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Item_Obtain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Item/Obtain/Item%d.png"), 13))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Slot */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Slot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Slot/Slot%d.png"), 4))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Potion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Potion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Potion/Potion%d.png"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Interactive */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Interactive"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Interactive/Interactive%d.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_Aggro */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Aggro"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Aggro/Aggro%d.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_LockOn */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LockOn"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LockOn/LockOn%d.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_UI_BossHP */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossHP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/BossHP/BossHP%d.png"), 5))))
		return E_FAIL;

	return	S_OK;
}

HRESULT CMainApp::Ready_Player()
{
	// Desc
	CPlayer::PLAYER_DESC tDesc = {};
	_char szModelTag[MAX_PATH] = "Prototype_Component_Model_Player";
	wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
	tDesc.strModelComTag = wstr;

	// Clone
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"), &tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Camera()
{
	// Camera_Free
	CCamera_Free::CAMERA_FREE_DESC tCameraFreeDesc{};
	tCameraFreeDesc.fMouseSensor = 0.5f;
	tCameraFreeDesc.fFovy = XMConvertToRadians(60.0f);
	tCameraFreeDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	tCameraFreeDesc.fNear = 0.1f;
	tCameraFreeDesc.fFar = 1000.0f;

	//tCameraFreeDesc.vEye = _float4(0.f, 16.f, -16.f, 1.f); 
	//tCameraFreeDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);		

	tCameraFreeDesc.vEye = _float4(-75.f, 13.f, 58.f, 1.f);
	tCameraFreeDesc.vAt = _float4(-75.f, 3.f, 68.f, 1.f);

	//tCameraFreeDesc.vEye = _float4(0.f, 0.02f, -61.f, 1.f);
	//tCameraFreeDesc.vAt = _float4(0.f, 0.02f, -51.f, 1.f);

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


	m_pGameInstance->Change_Camera(TEXT("Camera_Free"));

	m_pGameInstance->Set_Camera_Level(LEVEL_STATIC);

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

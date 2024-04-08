#include "stdafx.h"
#include "Loader.h"
#include <process.h>
#include "GameInstance.h"
#include "Camera_Free.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Monster.h"
#include "Map_Object.h"
#include "Dot.h"
//#include "Player.h"
//#include "Effect.h"
//#include "Sky.h"
#include "Model.h"
#include "Collider.h"

#include "Editor.h"
#include "Test_Object.h"
#include "Map.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext}
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	/* 로더에게 지정된 레벨을 준비 */
	CLoader*		pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Start()))
		return 1;

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_Critical_Section);

	/* 스레드 생성 */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Start()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = { 0 };

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Logo();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlay();
		break;
	case LEVEL_TOOL_MAP:
		hr = Loading_For_Tool_Map();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_Critical_Section);

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Texture_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	
	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");
		
	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");
	
	m_strLoadingText = TEXT("객체의 원형를(을) 로딩 중 입니다.");

	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	
	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	/* Prototype_Component_Texture_Terrain_Diffuse */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain_Brush */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	///* Prototype_Component_Texture_Player */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Player"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resources/Textures/Player/Player.png")))))
	//	return E_FAIL;

	///* Prototype_Component_Texture_Monster */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resources/Textures/Monster/Monster.png")))))
	//	return E_FAIL;

	///* Prototype_Component_Texture_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXCUBE, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
	//	return E_FAIL;

	///* Prototype_Component_Texture_Effect */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Effect"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEX2D, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
	//	return E_FAIL;
	//

	m_strLoadingText = TEXT("컴포넌트를(을) 로딩 중 입니다.");
	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;
		
	/* Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Data/Navigation/Navigation.dat")))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");
	_matrix		TransformMatrix = XMMatrixIdentity();

	///* For.Prototype_Component_Model_Fiona */
	//TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", TEXT("Prototype_Component_Model_Fiona"), TransformMatrix))))
	//	return E_FAIL;

	/* For.Prototype_Component_Model_Map */
	TransformMatrix = /*XMMatrixRotationY(XMConvertToRadians(180.0f));*/ XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Map"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Map3/Fox_God_map2.fbx", TEXT("Prototype_Component_Model_Map"), TransformMatrix))))
		return E_FAIL;

	///* For.Prototype_Component_Model_Fox */
	//TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fox"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fox/fox.fbx", TransformMatrix))))
	//	return E_FAIL;

	///* For.Prototype_Component_Model_Fox */
	//TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fox"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Cube/Cube.fbx", TransformMatrix))))
	//	return E_FAIL;

	// MAP OBJECT ================================================================================================================================

	///* Prototype_Component_Model_ForkLift */
	//TransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", TEXT("Prototype_Component_Model_ForkLift"), TransformMatrix))))
	//	return E_FAIL;

	/* Prototype_Component_Model_ForkLift */
	/*TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Map_Object"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Cube/Cube.fbx", TransformMatrix))))
		return E_FAIL;*/



	// ===========================================================================================================================================

	///* Prototype_Component_VIBuffer_Cube */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pGraphic_Device, 1000, 1000))))
	//	return E_FAIL;*/	
	
	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Shader_VtxPosNorTex */
  	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosNorTex.hlsl"), VTXPOSNORTEX::Elements, VTXPOSNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	///* For.Prototype_Component_Shader_VtxMap */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMap"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMap.hlsl"), VTXMAP::Elements, VTXMAP::iNumElements))))
	//	return E_FAIL;
	
	m_strLoadingText = TEXT("객체를(을) 로딩 중 입니다.");

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Player */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
	//	CPlayer::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	/* For.Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Map_Object */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map_Object"),
		CMap_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Map */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map"),
		CMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Effect */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"),
	//	CEffect::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///* For.Prototype_GameObject_Sky */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pGraphic_Device))))
	//	return E_FAIL;
	//

	

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Tool_Map()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	/* Prototype_Component_Texture_Terrain_Diffuse */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Texture_Terrain_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Texture_Terrain_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain_Brush */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Texture_Terrain_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	m_strLoadingText = TEXT("컴포넌트를(을) 로딩 중 입니다.");
	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_Navigation_Load */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Navigation_Load"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Data/Navigation/Nav_FOXGOD.dat")))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");
	_matrix		TransformMatrix = XMMatrixIdentity();
	///* For.Prototype_Component_Model_Fiona */
	//TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", TEXT("Prototype_Component_Model_Fiona"),TransformMatrix))))
	//	return E_FAIL;

	///* For.Prototype_Component_Model_Player */
	//TransformMatrix = /*XMMatrixRotationY(XMConvertToRadians(180.0f));*/ XMMatrixIdentity();
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Player"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/Player.fbx", TEXT("Prototype_Component_Model_Player"), TransformMatrix))))
	//	return E_FAIL;

	/* Prototype_Component_Model_ForkLift */
	//TransformMatrix = XMMatrixIdentity();
	/*TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", TEXT("Prototype_Component_Model_ForkLift"),TransformMatrix))))
		return E_FAIL;*/

	/* Prototype_Component_Model_Flower */
	TransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Flower"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Object/effigy (merge).fbx", TEXT("Prototype_Component_Model_Flower"), TransformMatrix))))
		return E_FAIL;

	///* For.Prototype_Component_Model_Map_Beach */
	//TransformMatrix = XMMatrixScaling(100.f, 100.f, 100.f);/*XMMatrixIdentity();*/
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Map_Beach"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Map/map_beach2.fbx", TEXT("Prototype_Component_Model_Map_Beach"), TransformMatrix))))
	//	return E_FAIL;

	/* For.Prototype_Component_Model_Map_FOXGOD */
	TransformMatrix = /*XMMatrixScaling(0.3f, 0.3f, 0.3f);*/ XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Map_FOXGOD"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Map3/Fox_God_map2.fbx", TEXT("Prototype_Component_Model_Map_FOXGOD"), TransformMatrix))))
		return E_FAIL;

	///* For.Prototype_Component_Model_Map_FOXGOD */
	//TransformMatrix =  XMMatrixIdentity();
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Map_Librarian"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Map4/Map_Librarian.fbx", TEXT("Prototype_Component_Model_Map_Librarian"), TransformMatrix))))
	//	return E_FAIL;

	/*TransformMatrix = XMMatrixScaling(20.f, 20.f, 20.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Object_Chest"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Object/Chest/Chest.fbx", TEXT("Prototype_Component_Model_Object_Chest"), TransformMatrix))))
		return E_FAIL;*/

	/* Boss */
	/*TransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Boss_Librarian"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Librarian/Librarian.fbx", TEXT("Prototype_Component_Model_Boss_Librarian"), TransformMatrix))))
		return E_FAIL;*/

	/*TransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Boss_Librarian_Effect_Orb"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Librarian/Effect/Orb/Orb.fbx", TEXT("Prototype_Component_Model_Boss_Librarian_Effect_Orb"), TransformMatrix))))
		return E_FAIL;*/

	/*TransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Boss_Librarian_Effect_Beam"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Librarian/Effect/Beam/Beam_Fin.fbx", TEXT("Prototype_Component_Model_Boss_Librarian_Effect_Beam"), TransformMatrix))))
		return E_FAIL;*/

	TransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Boss_Librarian_Effect_Slash_Horizon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Librarian/Effect/Slash/Slash_Horizon.fbx", TEXT("Prototype_Component_Model_Boss_Librarian_Effect_Slash_Horizon"), TransformMatrix))))
		return E_FAIL;

	/*TransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Boss_Librarian_Sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Librarian/Librarian_Sword.fbx", TEXT("Prototype_Component_Model_Boss_Librarian_Sword"), TransformMatrix))))
		return E_FAIL;*/

	///* Monster */
	//TransformMatrix = XMMatrixScaling(20.f, 20.f, 20.f);
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Monster_Bat"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Bat/Bat.fbx", TEXT("Prototype_Component_Model_Monster_Bat"), TransformMatrix))))
	//	return E_FAIL;

	//TransformMatrix = /*XMMatrixRotationY(XMConvertToRadians(180.0f));*/XMMatrixIdentity();
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Monster_Blob"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Blob/Blob.fbx", TEXT("Prototype_Component_Model_Monster_Blob"), TransformMatrix))))
	//	return E_FAIL;

	//TransformMatrix = /*XMMatrixRotationY(XMConvertToRadians(180.0f));*/XMMatrixIdentity();
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Monster_Blob_Normal"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Blob_Normal/Blob_Normal.fbx", TEXT("Prototype_Component_Model_Monster_Blob_Normal"), TransformMatrix))))
	//	return E_FAIL;

	//TransformMatrix = XMMatrixScaling(20.f, 20.f, 20.f);
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Monster_CowBot"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/CowBot/CowBot.fbx", TEXT("Prototype_Component_Model_Monster_CowBot"), TransformMatrix))))
	//	return E_FAIL;

	//TransformMatrix = /*XMMatrixScaling(20.f, 20.f, 20.f);*/XMMatrixIdentity();
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Monster_CowBot_Shield"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/CowBot/shield.fbx", TEXT("Prototype_Component_Model_Monster_CowBot_Shield"), TransformMatrix))))
	//	return E_FAIL;

	//TransformMatrix = /*XMMatrixScaling(20.f, 20.f, 20.f);*/XMMatrixIdentity();
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Monster_CowBot_Sword"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/CowBot/sword.fbx", TEXT("Prototype_Component_Model_Monster_CowBot_Sword"), TransformMatrix))))
	//	return E_FAIL;

	/*TransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Monster_Spinner"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Spinner/spinner.fbx", TEXT("Prototype_Component_Model_Monster_Spinner"), TransformMatrix))))
		return E_FAIL;*/

	/* For.Prototype_Component_Model_Map_Object */
	
	/*TransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Map_Object"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Object/Sword/Sword.fbx", TEXT("Prototype_Component_Model_Map_Object"), TransformMatrix))))
		return E_FAIL;*/

	/*TransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Weapon_Shield"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Object/shield/shield.fbx", TEXT("Prototype_Component_Model_Weapon_Shield"), TransformMatrix))))
		return E_FAIL;*/

	/*TransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Weapon_Shotgun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Object/shotgun/shotgun.fbx", TEXT("Prototype_Component_Model_Weapon_Shotgun"), TransformMatrix))))
		return E_FAIL;*/

	/*TransformMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Weapon_Wandbow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Object/Wandbow/Wandbow.fbx", TEXT("Prototype_Component_Model_Weapon_Wandbow"), TransformMatrix))))
		return E_FAIL;*/

	///* For.Prototype_Component_Model_Fox */
	//TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/Player.fbx", TransformMatrix))))
	//	return E_FAIL;

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Shader_VtxPosNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Shader_VtxPosNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosNorTex.hlsl"), VTXPOSNORTEX::Elements, VTXPOSNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	m_strLoadingText = TEXT("객체를(을) 로딩 중 입니다.");
	/* For.Prototype_GameObject_Editor */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Editor"),
		CEditor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Test_Object */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Test_Object"),
		CTest_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Map_Object */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map_Object"),
		CMap_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Map */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map"),
		CMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Dot */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dot"),
		CDot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("콜라이더를(을) 로딩 중 입니다.");
	/* Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CLoader"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CoUninitialize();

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}


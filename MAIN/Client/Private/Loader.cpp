#include "stdafx.h"
#include "Loader.h"
#include <process.h>
#include "GameInstance.h"
#include "BackGround.h"
#include "Terrain.h"

#include "Monster.h"
#include "Monster_Spinner.h"
#include "Monster_Blob.h"
#include "Monster_Bat.h"

//#include "Effect.h"
#include "Sky.h"
#include "Model.h"
#include "Collider.h"

#include "Player.h"
#include "Map.h"
#include "Map_Object.h"
#include "Test_Object.h"
#include "Editor.h"
#include "Player_Weapon.h"
#include "Animator.h"
#include "Particle_Blue.h"

#include "Camera_Free.h"
#include "Camera_Follow.h"
#include <fstream>

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

	/* Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;


	m_strLoadingText = TEXT("컴포넌트를(을) 로딩 중 입니다.");
	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Instance_Rect */
	CVIBuffer_Instance::INSTANCE_DESC		InstanceDesc{};

	InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vRange = _float3(20.f, 20.f, 20.f);
	InstanceDesc.vMinScale = _float3(1.f, 1.f, 1.f);
	InstanceDesc.vMaxScale = _float3(4.f, 4.f, 4.f);
	InstanceDesc.iNumInstance = 100;
	InstanceDesc.vLifeTime = _float2(0.f, 10.0f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Instance_Rect"),
		CVIBuffer_Instance_Rect::Create(m_pDevice, m_pContext, InstanceDesc))))
		return E_FAIL;

	/* Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Data/Navigation/Navigation.dat")))))
		return E_FAIL;

	m_strLoadingText = TEXT("콜라이더를(을) 로딩 중 입니다.");
	/* Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* Prototype_Component_Collider_SPHERE */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;
	
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

	/* For.Prototype_Component_Shader_VtxInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance.hlsl"), VTXINSTANCE::Elements, VTXINSTANCE::iNumElements))))
		return E_FAIL;

	
	m_strLoadingText = TEXT("객체를(을) 로딩 중 입니다.");

	/* For.Prototype_GameObject_Particle_Blue */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Blue"),
		CParticle_Blue::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Editor */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Editor"),
		CEditor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_
	*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Follow"),
		CCamera_Follow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_Spinner */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Spinner"),
		CMonster_Spinner::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_Blob */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Blob"),
		CMonster_Blob::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_Blob */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Bat"),
		CMonster_Bat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Map_Object */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map_Object"),
		CMap_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Map */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map"),
		CMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_Player_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Player_Weapon"),
		CPlayer_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	Load_NonAnim_Model("../Bin/Resources/Data/Model/Weapon_Stick.dat");
	Load_NonAnim_Model("../Bin/Resources/Data/Model/Weapon_Shield.dat");
	Load_NonAnim_Model("../Bin/Resources/Data/Model/Weapon_Sword.dat");
	Load_NonAnim_Model("../Bin/Resources/Data/Model/Weapon_Shotgun.dat");
	//Load_NonAnim_Model("../Bin/Resources/Data/Model/Map_Beach.dat");
	Load_NonAnim_Model("../Bin/Resources/Data/Model/Map_FOXGOD.dat");
	Load_Anim_Model("../Bin/Resources/Data/Model/Monster.dat");
	Load_Anim_Model("../Bin/Resources/Data/Model/Player.dat");

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_NonAnim_Model(const string& strDataPath)
{
	ifstream fin;
	//const string temp = "../Bin/Resources/Data/Model/Stick.dat";
	fin.open(strDataPath.c_str(), ios::in | ios::binary);

	_uint iObjectCount;
	fin.read(reinterpret_cast<char*>(&iObjectCount), sizeof(_uint));
	for (size_t i = 0; i < iObjectCount; ++i)
	{
		MODELFILE tModelFile = {};

		fin.read(reinterpret_cast<char*>(&tModelFile.szModelComTag), sizeof(_char) * MAX_PATH);

		// Type
		fin.read(reinterpret_cast<char*>(&tModelFile.iType), sizeof(_uint));

		// Mesh
		fin.read(reinterpret_cast<char*>(&tModelFile.iNumMeshes), sizeof(_uint));
		for (size_t j = 0; j < tModelFile.iNumMeshes; ++j)
		{
			MESHFILE tMeshFile{};
			fin.read(reinterpret_cast<char*>(&tMeshFile.szName), sizeof(_char) * MAX_PATH);
			fin.read(reinterpret_cast<char*>(&tMeshFile.iMaterialIndex), sizeof(_uint));

			fin.read(reinterpret_cast<char*>(&tMeshFile.iNumFaces), sizeof(_uint));

			fin.read(reinterpret_cast<char*>(&tMeshFile.iNumBones), sizeof(_uint));
			for (size_t k = 0; k < tMeshFile.iNumBones; ++k)
			{
				_uint	iBoneIndex{};
				fin.read(reinterpret_cast<char*>(&iBoneIndex), sizeof(_uint));
				tMeshFile.Bones.push_back(iBoneIndex);
			}

			_uint iNumVertices{};
			fin.read(reinterpret_cast<char*>(&iNumVertices), sizeof(_uint));
			tMeshFile.iNumVertices = iNumVertices;

			if (CModel::TYPE_NONANIM == tModelFile.iType)
			{
				tMeshFile.pMeshVertices = new VTXMESH[iNumVertices];
				ZeroMemory(tMeshFile.pMeshVertices, sizeof(VTXMESH) * iNumVertices);
				for (size_t k = 0; k < iNumVertices; ++k)
				{
					VTXMESH tMesh{};
					fin.read(reinterpret_cast<char*>(&tMesh), sizeof(VTXMESH));
					memcpy(&tMeshFile.pMeshVertices[k], &tMesh, sizeof(VTXMESH));
				}
			}
			else
			{
				tMeshFile.pAnimMeshVertices = new VTXANIMMESH[iNumVertices];
				ZeroMemory(tMeshFile.pAnimMeshVertices, sizeof(VTXANIMMESH) * iNumVertices);
				for (size_t k = 0; k < iNumVertices; ++k)
				{
					VTXANIMMESH tMesh{};
					fin.read(reinterpret_cast<char*>(&tMesh), sizeof(VTXANIMMESH));
					memcpy(&tMeshFile.pAnimMeshVertices[k], &tMesh, sizeof(VTXANIMMESH));
				}
			}

			_uint iNumIndices{};
			fin.read(reinterpret_cast<char*>(&iNumIndices), sizeof(_uint));
			tMeshFile.iNumIndices = iNumIndices;

			tMeshFile.pIndices = new _uint[iNumIndices];
			ZeroMemory(tMeshFile.pIndices, sizeof(_uint) * iNumIndices);
			for (size_t k = 0; k < iNumIndices; ++k)
			{
				_uint iIndex{};
				fin.read(reinterpret_cast<char*>(&iIndex), sizeof(_uint));
				memcpy(&tMeshFile.pIndices[k], &iIndex, sizeof(_uint));
			}

			fin.read(reinterpret_cast<char*>(&tMeshFile.iNumOffsetMatrices), sizeof(_uint));
			for (size_t k = 0; k < tMeshFile.iNumOffsetMatrices; ++k)
			{
				_float4x4	OffsetMatrix{};
				fin.read(reinterpret_cast<char*>(&OffsetMatrix), sizeof(_float4x4));
				tMeshFile.OffsetMatrices.push_back(OffsetMatrix);
			}
			tModelFile.Meshes.push_back(tMeshFile);
		}

		// Material
		fin.read(reinterpret_cast<char*>(&tModelFile.iNumMaterials), sizeof(_uint));
		vector<_uint> NumTextures;
		vector<vector<MATERIALFILE>> vecMaterials;
		for (size_t j = 0; j < tModelFile.iNumMaterials; ++j)
		{
			// NumTextures
			_uint iNum = 0;
			fin.read(reinterpret_cast<char*>(&iNum), sizeof(_uint));
			tModelFile.NumTextures.push_back(iNum);

			// Materials
			vector<MATERIALFILE> Materials;
			for (size_t k = 0; k < tModelFile.NumTextures[j]; ++k)
			{
				MATERIALFILE tMaterial = {};
				fin.read(reinterpret_cast<char*>(&tMaterial), sizeof(MATERIALFILE));
				Materials.push_back(tMaterial);
			}
			tModelFile.Materials.push_back(Materials);
		}

		// Bone
		fin.read(reinterpret_cast<char*>(&tModelFile.TransformMatrix), sizeof(_float4x4));
		fin.read(reinterpret_cast<char*>(&tModelFile.iNumBones), sizeof(_uint));
		for (size_t j = 0; j < tModelFile.iNumBones; ++j)
		{
			BONEFILE tBoneFile = {};
			fin.read(reinterpret_cast<char*>(&tBoneFile), sizeof(BONEFILE));
			tModelFile.Bones.push_back(tBoneFile);
		}

		// Animation
		fin.read(reinterpret_cast<char*>(&tModelFile.iNumAnimations), sizeof(_uint));
		fin.read(reinterpret_cast<char*>(&tModelFile.iCurrentAnimIndex), sizeof(_uint));
		fin.read(reinterpret_cast<char*>(&tModelFile.isLoop), sizeof(_bool));
		for (size_t j = 0; j < tModelFile.iNumAnimations; ++j)
		{
			ANIMFILE tAnimFile{};
			fin.read(reinterpret_cast<char*>(&tAnimFile.szName), sizeof(_char) * MAX_PATH);

			fin.read(reinterpret_cast<char*>(&tAnimFile.fDuration), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&tAnimFile.fTicksPerSecond), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&tAnimFile.fTrackPosition), sizeof(_float));

			fin.read(reinterpret_cast<char*>(&tAnimFile.iNumChannels), sizeof(_uint));
			for (size_t k = 0; k < tAnimFile.iNumChannels; ++k)
			{
				CHANNELFILE tChannelFile = {};
				fin.read(reinterpret_cast<char*>(&tChannelFile.szName), sizeof(_char) * MAX_PATH);
				fin.read(reinterpret_cast<char*>(&tChannelFile.iBoneIndex), sizeof(_int));

				fin.read(reinterpret_cast<char*>(&tChannelFile.iNumKeyFrames), sizeof(_uint));
				for (size_t l = 0; l < tChannelFile.iNumKeyFrames; ++l)
				{
					KEYFRAME tKeyFrame = {};
					fin.read(reinterpret_cast<char*>(&tKeyFrame), sizeof(KEYFRAME));
					tChannelFile.KeyFrames.push_back(tKeyFrame);
				}
				tAnimFile.Channels.push_back(tChannelFile);
			}

			tModelFile.Animations.push_back(tAnimFile);
		}

		for (size_t j = 0; j < 512; ++j)
			fin.read(reinterpret_cast<char*>(&tModelFile.MeshBoneMatrices[j]), sizeof(_float4x4));

		// TYPE
		CModel::TYPE eType = (CModel::TYPE)tModelFile.iType;

		// Model Tag
		wstring wstr(&tModelFile.szModelComTag[0], &tModelFile.szModelComTag[MAX_PATH]);

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, wstr,
			CModel::Create(m_pDevice, m_pContext, eType, &tModelFile))))
			return E_FAIL;

		for(size_t j = 0; j < tModelFile.iNumMeshes; ++j)
			Safe_Delete_Array(tModelFile.Meshes[j].pMeshVertices);

		for (size_t j = 0; j < tModelFile.iNumMeshes; ++j)
			Safe_Delete_Array(tModelFile.Meshes[j].pAnimMeshVertices);

		for (size_t j = 0; j < tModelFile.iNumMeshes; ++j)
			Safe_Delete_Array(tModelFile.Meshes[j].pIndices);

		int a = 0;
	}

	fin.close();
}

HRESULT CLoader::Load_Anim_Model(const string& strDataPath)
{
	ifstream fin;
	fin.open(strDataPath.c_str(), ios::in | ios::binary);

	_uint iObjectCount = 0;
	fin.read(reinterpret_cast<char*>(&iObjectCount), sizeof(_uint));
	for (size_t i = 0; i < iObjectCount; ++i)
	{
		MODELFILE tModelFile = {};

		fin.read(reinterpret_cast<char*>(&tModelFile.szModelComTag), sizeof(_char) * MAX_PATH);

		// Type
		fin.read(reinterpret_cast<char*>(&tModelFile.iType), sizeof(_uint));

		// Mesh
		fin.read(reinterpret_cast<char*>(&tModelFile.iNumMeshes), sizeof(_uint));
		for (size_t j = 0; j < tModelFile.iNumMeshes; ++j)
		{
			MESHFILE tMeshFile{};
			fin.read(reinterpret_cast<char*>(&tMeshFile.szName), sizeof(_char) * MAX_PATH);
			fin.read(reinterpret_cast<char*>(&tMeshFile.iMaterialIndex), sizeof(_uint));

			fin.read(reinterpret_cast<char*>(&tMeshFile.iNumFaces), sizeof(_uint));

			fin.read(reinterpret_cast<char*>(&tMeshFile.iNumBones), sizeof(_uint));
			for (size_t k = 0; k < tMeshFile.iNumBones; ++k)
			{
				_uint	iBoneIndex{};
				fin.read(reinterpret_cast<char*>(&iBoneIndex), sizeof(_uint));
				tMeshFile.Bones.push_back(iBoneIndex);
			}

			_uint iNumVertices{};
			fin.read(reinterpret_cast<char*>(&iNumVertices), sizeof(_uint));
			tMeshFile.iNumVertices = iNumVertices;

			if (CModel::TYPE_NONANIM == tModelFile.iType)
			{
				tMeshFile.pMeshVertices = new VTXMESH[iNumVertices];
				ZeroMemory(tMeshFile.pMeshVertices, sizeof(VTXMESH) * iNumVertices);
				for (size_t k = 0; k < iNumVertices; ++k)
				{
					VTXMESH tMesh{};
					fin.read(reinterpret_cast<char*>(&tMesh), sizeof(VTXMESH));
					memcpy(&tMeshFile.pMeshVertices[k], &tMesh, sizeof(VTXMESH));
				}
			}
			else
			{
				tMeshFile.pAnimMeshVertices = new VTXANIMMESH[iNumVertices];
				ZeroMemory(tMeshFile.pAnimMeshVertices, sizeof(VTXANIMMESH) * iNumVertices);
				for (size_t k = 0; k < iNumVertices; ++k)
				{
					VTXANIMMESH tMesh{};
					fin.read(reinterpret_cast<char*>(&tMesh), sizeof(VTXANIMMESH));
					memcpy(&tMeshFile.pAnimMeshVertices[k], &tMesh, sizeof(VTXANIMMESH));
				}
			}

			_uint iNumIndices{};
			fin.read(reinterpret_cast<char*>(&iNumIndices), sizeof(_uint));
			tMeshFile.iNumIndices = iNumIndices;

			tMeshFile.pIndices = new _uint[iNumIndices];
			ZeroMemory(tMeshFile.pIndices, sizeof(_uint) * iNumIndices);
			for (size_t k = 0; k < iNumIndices; ++k)
			{
				_uint iIndex{};
				fin.read(reinterpret_cast<char*>(&iIndex), sizeof(_uint));
				memcpy(&tMeshFile.pIndices[k], &iIndex, sizeof(_uint));
			}

			fin.read(reinterpret_cast<char*>(&tMeshFile.iNumOffsetMatrices), sizeof(_uint));
			for (size_t k = 0; k < tMeshFile.iNumOffsetMatrices; ++k)
			{
				_float4x4	OffsetMatrix{};
				fin.read(reinterpret_cast<char*>(&OffsetMatrix), sizeof(_float4x4));
				tMeshFile.OffsetMatrices.push_back(OffsetMatrix);
			}
			tModelFile.Meshes.push_back(tMeshFile);
		}

		// Material
		fin.read(reinterpret_cast<char*>(&tModelFile.iNumMaterials), sizeof(_uint));
		vector<_uint> NumTextures;
		vector<vector<MATERIALFILE>> vecMaterials;
		for (size_t j = 0; j < tModelFile.iNumMaterials; ++j)
		{
			// NumTextures
			_uint iNum = 0;
			fin.read(reinterpret_cast<char*>(&iNum), sizeof(_uint));
			tModelFile.NumTextures.push_back(iNum);

			// Materials
			vector<MATERIALFILE> Materials;
			for (size_t k = 0; k < tModelFile.NumTextures[j]; ++k)
			{
				MATERIALFILE tMaterial = {};
				fin.read(reinterpret_cast<char*>(&tMaterial), sizeof(MATERIALFILE));
				Materials.push_back(tMaterial);
			}
			tModelFile.Materials.push_back(Materials);
		}

		// Bone
		fin.read(reinterpret_cast<char*>(&tModelFile.TransformMatrix), sizeof(_float4x4));
		fin.read(reinterpret_cast<char*>(&tModelFile.iNumBones), sizeof(_uint));
		for (size_t j = 0; j < tModelFile.iNumBones; ++j)
		{
			BONEFILE tBoneFile = {};
			fin.read(reinterpret_cast<char*>(&tBoneFile), sizeof(BONEFILE));
			tModelFile.Bones.push_back(tBoneFile);
		}

		// Animation
		fin.read(reinterpret_cast<char*>(&tModelFile.iNumAnimations), sizeof(_uint));
		fin.read(reinterpret_cast<char*>(&tModelFile.iCurrentAnimIndex), sizeof(_uint));
		fin.read(reinterpret_cast<char*>(&tModelFile.isLoop), sizeof(_bool));
		for (size_t j = 0; j < tModelFile.iNumAnimations; ++j)
		{
			ANIMFILE tAnimFile{};
			fin.read(reinterpret_cast<char*>(&tAnimFile.szName), sizeof(_char) * MAX_PATH);

			fin.read(reinterpret_cast<char*>(&tAnimFile.fDuration), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&tAnimFile.fTicksPerSecond), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&tAnimFile.fTrackPosition), sizeof(_float));

			fin.read(reinterpret_cast<char*>(&tAnimFile.iNumChannels), sizeof(_uint));
			for (size_t k = 0; k < tAnimFile.iNumChannels; ++k)
			{
				CHANNELFILE tChannelFile = {};
				fin.read(reinterpret_cast<char*>(&tChannelFile.szName), sizeof(_char) * MAX_PATH);
				fin.read(reinterpret_cast<char*>(&tChannelFile.iBoneIndex), sizeof(_int));

				fin.read(reinterpret_cast<char*>(&tChannelFile.iNumKeyFrames), sizeof(_uint));
				for (size_t l = 0; l < tChannelFile.iNumKeyFrames; ++l)
				{
					KEYFRAME tKeyFrame = {};
					fin.read(reinterpret_cast<char*>(&tKeyFrame), sizeof(KEYFRAME));
					tChannelFile.KeyFrames.push_back(tKeyFrame);
				}
				tAnimFile.Channels.push_back(tChannelFile);
			}

			tModelFile.Animations.push_back(tAnimFile);
		}

		for (size_t j = 0; j < 512; ++j)
			fin.read(reinterpret_cast<char*>(&tModelFile.MeshBoneMatrices[j]), sizeof(_float4x4));

		// TYPE
		CModel::TYPE eType = (CModel::TYPE)tModelFile.iType;

		// Model Tag
		wstring wstr(&tModelFile.szModelComTag[0], &tModelFile.szModelComTag[MAX_PATH]);

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, wstr,
			CAnimator::Create(m_pDevice, m_pContext, &tModelFile))))
			return E_FAIL;

		for (size_t j = 0; j < tModelFile.iNumMeshes; ++j)
			Safe_Delete_Array(tModelFile.Meshes[j].pMeshVertices);

		for (size_t j = 0; j < tModelFile.iNumMeshes; ++j)
			Safe_Delete_Array(tModelFile.Meshes[j].pAnimMeshVertices);

		for (size_t j = 0; j < tModelFile.iNumMeshes; ++j)
			Safe_Delete_Array(tModelFile.Meshes[j].pIndices);
	}

	fin.close();

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


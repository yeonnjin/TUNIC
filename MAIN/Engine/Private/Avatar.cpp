#include "Avatar.h"
#include "GameInstance.h"

#include "Mesh.h"

CAvatar::CAvatar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
	, m_isCloned{ false }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CAvatar::CAvatar(const CAvatar& rhs)
	: m_pDevice{ rhs.m_pDevice }
	, m_pContext{ rhs.m_pContext }
	, m_pGameInstance{ rhs.m_pGameInstance }
	, m_isCloned{ true }
	, m_eModelType{ rhs.m_eModelType }
	, m_iNumMeshes{ rhs.m_iNumMeshes }
	, m_Meshes{ rhs.m_Meshes }
	, m_iNumMaterials{ rhs.m_iNumMaterials }
	, m_NumTextures{ rhs.m_NumTextures }
	, m_Materials{ rhs.m_Materials }
	, m_TransformationMatrix{ rhs.m_TransformationMatrix }
	, m_iNumAnimations{ rhs.m_iNumAnimations }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	// 깊은 복사
	for (auto& pPrototypeAnimation : rhs.m_Animations)
		m_Animations.push_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	// 얕은 복사
	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& tMaterial : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(tMaterial.MaterialTextures[i]);
	}
}

void CAvatar::Set_ParentBoneIndex(_int iIndex)
{
	for (auto& pBone : m_Bones)
		pBone->Set_ParentBoneIndex(iIndex);
}

void CAvatar::Set_isUseTransformaion(_bool isUse)
{
	for (auto& pBone : m_Bones)
		pBone->Set_isUseTransformaion(isUse);
}

CBone* CAvatar::Get_Bone_Ptr(_uint iBoneIndex) const
{
	auto iter = m_Bones.begin();
	for (size_t i = 0; i < iBoneIndex; i++)
		++iter;

	return *iter;
}

_float4 CAvatar::Get_Bone_Position(_uint iBoneIndex) const
{
	auto iter = m_Bones.begin();
	for (size_t i = 0; i < iBoneIndex; i++)
		++iter;

	return (*iter)->Get_Position();
}

HRESULT CAvatar::Initialize_Prototype(TYPE eType, MODELFILE* pModelFile)
{
	if (nullptr == pModelFile)
		return E_FAIL;

	m_eModelType = eType;

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixIdentity());

	/* 전체 뼈 생성 */
	if (FAILED(Ready_Bones(pModelFile->iNumBones, pModelFile->Bones)))
		return E_FAIL;

	/* 모델을 구성하는 메쉬 생성 */
	if (FAILED(Ready_Meshes(pModelFile->iNumMeshes, pModelFile->Meshes)))
		return E_FAIL;

	/* 머테리얼 생성 */
	if (FAILED(Ready_Materials(pModelFile->iNumMaterials, pModelFile->NumTextures, pModelFile->Materials)))
		return E_FAIL;

	/* 애니메이션 생성 */
	if (FAILED(Ready_Animations(pModelFile->iNumAnimations, pModelFile->Animations)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAvatar::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CAvatar::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	ZeroMemory(m_MeshBoneMatrices, sizeof(_float4x4) * 512);

	m_Meshes[iMeshIndex]->Stock_Matrices(m_Bones, m_MeshBoneMatrices);

	return pShader->Bind_Matrices(pConstantName, m_MeshBoneMatrices, 512);
}

HRESULT CAvatar::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, AITEXTURETYPE eTextureType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	// 바인드 할 메쉬의 머테리얼 인덱스
	_uint iMeshMaterialIndex = { m_Meshes[iMeshIndex]->Get_MaterialIndex() };

	if (iMeshMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	// 해당 머테리얼 인덱스에 해당하는 구조체에서 원하는 타입의 텍스쳐 클래스 주소 받아옴
	CTexture* pTexture = { m_Materials[iMeshMaterialIndex].MaterialTextures[eTextureType] };
	if (nullptr != pTexture)
		if (FAILED(pTexture->Bind_ShaderResource(pShader, pConstantName)))

			return E_FAIL;

	return S_OK;
}

HRESULT CAvatar::Render(_uint iMeshIndex)
{
	// 메쉬들을 순차적으로 렌더 
	// iMeshIndex을 Get_NumMeshes()로 Client에서 받아서 루프 수행
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

// Ready =======================================================================================================================================

HRESULT CAvatar::Ready_Meshes(_uint iNumMeshes, vector<MESHFILE>& pMeshFile)
{
	m_iNumMeshes = iNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, &pMeshFile[i], m_Bones);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CAvatar::Ready_Materials(_uint iNumMaterials, vector<_uint>& pNumTextures, vector<vector<MATERIALFILE>>& pMaterialFile)
{
	m_iNumMaterials = iNumMaterials;

	// FACE, BODY, ...
	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MESH_MATERIAL			MeshMaterial{};

		//_uint iNumTextures = pNumTextures[i];
		_uint iCount = 0;

		for (size_t j = TEX_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			if (0 != pMaterialFile[i].size() && j == pMaterialFile[i][iCount].iTextureIndex)
			{
				_char			szFullPath[MAX_PATH] = { "" };
				strcpy_s(szFullPath, pMaterialFile[i][iCount].szTexturePath);

				// ..\Bin\Resources\Models\Fiona\ 
				_tchar			szPerfectPath[MAX_PATH] = { L"" };
				MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szPerfectPath, MAX_PATH);

				MeshMaterial.MaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath);
				if (nullptr == MeshMaterial.MaterialTextures[j])
					return E_FAIL;

				++iCount;
				if (iCount == pNumTextures[i])
					break;
			}
			else
				continue;
		}

		m_Materials.push_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CAvatar::Ready_Bones(_uint iNumBones, vector<BONEFILE>& pBoneFile)
{
	for (size_t i = 0; i < iNumBones; ++i)
	{
		CBone* pBone = CBone::Create(&pBoneFile[i]);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CAvatar::Ready_Animations(_uint iNumAnimations, vector<ANIMFILE>& pAnimFile)
{
	m_iNumAnimations = iNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(&pAnimFile[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

// ============================================================================================================================================

CAvatar* CAvatar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, MODELFILE* pModelFile)
{
	CAvatar* pInstance = new CAvatar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFile)))
	{
		MSG_BOX(TEXT("Failed To Create : CAvatar"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CAvatar* CAvatar::Clone(void* pArg)
{
	CAvatar* pInstance = new CAvatar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CAvatar"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAvatar::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& tMaterial : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(tMaterial.MaterialTextures[i]);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	
	m_Meshes.clear();
	
}

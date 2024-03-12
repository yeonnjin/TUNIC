#include "Model.h"

#include "Bone.h"
#include "Mesh.h"
#include "Channel.h"
#include "Animation.h"

#include "Shader.h"
#include "Texture.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CModel::CModel(const CModel& rhs)
    : CComponent{ rhs }
	, m_eModelType{ rhs.m_eModelType }
	, m_iNumMeshes{ rhs.m_iNumMeshes }
	, m_Meshes{ rhs.m_Meshes }
	, m_iNumMaterials{ rhs.m_iNumMaterials }
	, m_NumTextures{ rhs.m_NumTextures }
	, m_Materials{ rhs.m_Materials }
	, m_TransformMatrix{ rhs.m_TransformMatrix }
	, m_iNumAnimations{ rhs.m_iNumAnimations }
	, m_iNumBones{ rhs.m_iNumBones }
{
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

HRESULT CModel::Initialize_Prototype(TYPE eType, MODELFILE* pModelFile)
{
	if (nullptr == pModelFile)
		return E_FAIL;

	m_eModelType = eType;

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());

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

HRESULT CModel::Initialize(void* pArg)
{
	m_ChannelStates[STATE_PREV].resize(m_iNumBones);
	m_ChannelStates[STATE_CUR].resize(m_iNumBones);

    return S_OK;
}

void CModel::Set_Animation(_uint iAnimIndex, _bool isLoop)
{
	// Linear
	m_isBlending = true;
	m_iPrevAnimIndex = m_iCurrentAnimIndex;

	m_iCurrentAnimIndex = iAnimIndex;
	m_isLoop = isLoop;
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	ZeroMemory(m_MeshBoneMatrices, sizeof(_float4x4) * 512);

	m_Meshes[iMeshIndex]->Stock_Matrices(m_Bones, m_MeshBoneMatrices);

	return pShader->Bind_Matrices(pConstantName, m_MeshBoneMatrices, 512);
}

_bool CModel::Check_Picking(const CTransform* pTransform) const
{
	for (size_t i = 0; i < m_iNumMeshes; ++i)
	{
		_float3 vPickingPos = m_Meshes[i]->Compute_Picking(pTransform);

		if (!(0.f == vPickingPos.x && 0.f == vPickingPos.y && 0.f == vPickingPos.z))
			return true;
	}

	return false;
}

HRESULT CModel::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, AITEXTURETYPE eTextureType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	// 바인드 할 메쉬의 머테리얼 인덱스
	_uint iMeshMaterialIndex = { m_Meshes[iMeshIndex]->Get_MaterialIndex() };

	if (iMeshMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	// 해당 머테리얼 인덱스에 해당하는 구조체에서 원하는 타입의 텍스쳐 클래스 주소 받아옴
	// m_Materials : vector<MESH_MATERIAL>;
	// MESH_MATERIAL : class CTexture* MaterialTextures[AI_TEXTURE_TYPE_MAX];
	CTexture* pTexture = { m_Materials[iMeshMaterialIndex].MaterialTextures[eTextureType] };
	if(nullptr != pTexture)
		pTexture->Bind_ShaderResource(pShader, pConstantName);

	return S_OK;
}

HRESULT CModel::Play_Animation(_float fTimeDelta)
{
	// 애니메이션 블렌딩 중일 때
	if (true == m_isBlending)
	{
		if (true == is_Blended(fTimeDelta))
			m_isBlending = false;
	}
	else
	{
		/* 현재 애니메이션에 맞는 뼈의 상태(m_TransformationMatrix)를 갱신 */
		m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones, m_isLoop);

		for (auto& pBone : m_Bones)
			pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
	}

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	// 메쉬들을 순차적으로 렌더 
	// iMeshIndex을 Get_NumMeshes()로 Client에서 받아서 루프 수행
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

_bool CModel::is_Blended(_float fTimeDelta)
{
	static _bool isBlending = false;

	if (false == isBlending)
	{
		//m_ChannelStates[STATE_PREV].clear();
		//m_ChannelStates[STATE_CUR].clear();

		for (size_t i = 0; i < m_iNumBones; ++i)
		{
			// PREV
			_float4x4 BoneFloat4x4 = m_Bones[i]->Get_TransformationMatrix();
			_matrix BoneMatrix = XMLoadFloat4x4(&BoneFloat4x4);

			_vector vScale, vRotation, vTranslation;

			bool result = XMMatrixDecompose(&vScale, &vRotation, &vTranslation, BoneMatrix);

			vScale = { 1.f, 1.f, 1.f };

			 XMStoreFloat3(&m_ChannelStates[STATE_PREV][i].vScale, vScale);
			 XMStoreFloat4(&m_ChannelStates[STATE_PREV][i].vRotation, vRotation);
			 XMStoreFloat3(&m_ChannelStates[STATE_PREV][i].vTranslation, vTranslation);		

			 XMStoreFloat3(&m_ChannelStates[STATE_CUR][i].vScale, vScale);
			 XMStoreFloat4(&m_ChannelStates[STATE_CUR][i].vRotation, vRotation);
			 XMStoreFloat3(&m_ChannelStates[STATE_CUR][i].vTranslation, vTranslation);
		}


		// PREV
		_uint iPrevNumChannels = m_Animations[m_iPrevAnimIndex]->Get_NumChannels();
		for (size_t i = 0; i < iPrevNumChannels; ++i)
		{
			CChannel* pChannel = m_Animations[m_iPrevAnimIndex]->Get_Channel(i);
			m_ChannelStates[STATE_PREV][pChannel->Get_BoneIndex()] = *pChannel->Get_ChannelState();
		}

		// CUR
		m_Animations[m_iCurrentAnimIndex]->Invalidate_Blending(fTimeDelta, m_Bones, true);
		_uint iCurNumChannels = m_Animations[m_iCurrentAnimIndex]->Get_NumChannels();
		for (size_t i = 0; i < iCurNumChannels; ++i)
		{
			CChannel* pChannel = m_Animations[m_iCurrentAnimIndex]->Get_Channel(i);
			m_ChannelStates[STATE_CUR][pChannel->Get_BoneIndex()] = *pChannel->Get_ChannelState();
		}

		isBlending = true;
	}
	else
	{
		if (Animation_Blending(fTimeDelta))
		{
			isBlending = false;
			return true;
		}
	}

	return false;
}

_bool CModel::Animation_Blending(_float fTimeDelta)
{
	static _float fTime = 0.f;
	static _float fBlendTime = 0.2f;

	_float3     vScale;
	_float4     vRotation;
	_float3     vTranslation;

	fTime += fTimeDelta;
	if (fTime > fBlendTime)
	{
		fTime = 0.f;
		return true;
	}

	_float fRatio = fTime / fBlendTime;

	// 선형보간
	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&m_ChannelStates[STATE_PREV][i].vScale), XMLoadFloat3(&m_ChannelStates[STATE_CUR][i].vScale), fRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&m_ChannelStates[STATE_PREV][i].vRotation), XMLoadFloat4(&m_ChannelStates[STATE_CUR][i].vRotation), fRatio));
		XMStoreFloat3(&vTranslation, XMVectorLerp(XMLoadFloat3(&m_ChannelStates[STATE_PREV][i].vTranslation), XMLoadFloat3(&m_ChannelStates[STATE_CUR][i].vTranslation), fRatio));

		_matrix     TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f));

		m_Bones[i]->Set_TransformationMatrix(TransformationMatrix);
	}

	for (auto& pBone : m_Bones)
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));

	return false;
}

HRESULT CModel::Ready_Meshes(_uint iNumMeshes, vector<MESHFILE>& pMeshFile)
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

HRESULT CModel::Ready_Materials(_uint iNumMaterials, vector<_uint>& pNumTextures, vector<vector<MATERIALFILE>>& pMaterialFile)
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

			//for (size_t k = 0; k < pNumTextures[i]; ++k)
			//{
			//	if (j == pMaterialFile[i][k].iTextureIndex)
			//	{
			//		_char			szFullPath[MAX_PATH] = { "" };
			//		strcpy_s(szFullPath, pMaterialFile[i][j].szTexturePath);

			//		// ..\Bin\Resources\Models\Fiona\ 
			//		_tchar			szPerfectPath[MAX_PATH] = { L"" };
			//		MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szPerfectPath, MAX_PATH);

			//		MeshMaterial.MaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath);
			//		if (nullptr == MeshMaterial.MaterialTextures[j])
			//			return E_FAIL;

			//	}
			//}
			
		}


		//// DIFFUSE, ...
		//for (size_t j = TEX_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; j++)
		//{			
		//	if (j != pMaterialFile[i].iTextureIndex)
		//		continue;

		//	_char			szFullPath[MAX_PATH] = { "" };
		//	strcpy_s(szFullPath, pMaterialFile[i].szTexturePath);

		//	// ..\Bin\Resources\Models\Fiona\ 
		//	_tchar			szPerfectPath[MAX_PATH] = { L"" };

		//	MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szPerfectPath, MAX_PATH);

		//	MeshMaterial.MaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath);
		//	if (nullptr == MeshMaterial.MaterialTextures[j])
		//		return E_FAIL;
		//}

		m_Materials.push_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(_uint iNumBones, vector<BONEFILE>& pBoneFile)
{
	for (size_t i = 0; i < iNumBones; ++i)
	{
		CBone* pBone = CBone::Create(&pBoneFile[i]);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}

	// Linear
	m_iNumBones = iNumBones;

	return S_OK;
}

HRESULT CModel::Ready_Animations(_uint iNumAnimations, vector<ANIMFILE>& pAnimFile)
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

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, MODELFILE* pModelFile)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFile)))
	{
		MSG_BOX(TEXT("Failed To Create : CModel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CModel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
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


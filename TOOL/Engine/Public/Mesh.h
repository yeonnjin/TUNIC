#pragma once

#include "VIBuffer.h"
#include "Model.h"

/* 디자이너 분들이 저장해준 정점과 인덱스의 정보를 바탕으로 정점, 인덱스 버퍼 생성 */

BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint	Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	// File
	MESHFILE* Get_MeshFile() { Ready_MeshFile(); return &m_tMeshFile; }

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, const vector<CBone*>& Bones, _fmatrix TransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT			Stock_Matrices(const vector<CBone*>& Bones, _float4x4* pMeshBoneMatrices);

public:
	_float3			Compute_Picking(const class CTransform* pTransform) const;

private:
	_char				m_szName[MAX_PATH] = { "" };

	// 이 메쉬는 모델에서 로드 해놓은 머테리얼들 중 몇 번째 머테리얼을 이용하는가
	_uint				m_iMaterialIndex = { 0 };		

	_uint				m_iNumFaces = { 0 };

	// 뼈의 인덱스 : 모델이 가지고 있는 전체 뼈들 중, 메쉬가 사용하고 있는 뼈의 인덱스들
	_uint				m_iNumBones = { 0 };
	vector<_uint>		m_Bones;

	_uint*				m_pIndices = { nullptr };

	// 현재 메쉬에 영향을 주는 뼈들의 순서대로 오프셋 저장
	vector<_float4x4>	m_OffsetMatrices;

	// FILE
	MESHFILE			m_tMeshFile;

private:
	HRESULT Ready_Vertices_For_NonAnimModel(const aiMesh* pAIMesh, _fmatrix TransformationMatrix);
	HRESULT Ready_Vertices_For_AnimModel(const aiMesh* pAIMesh, const vector<CBone*>& Bones);
	HRESULT Ready_MeshFile();

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, const vector<class CBone*>& Bones, _fmatrix TransformMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END
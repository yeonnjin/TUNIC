#pragma once

#include "Component.h"
#include "Animation.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	// Set
	void	Set_Animation(_uint iAnimIndex, _bool isLoop) {
		m_iCurrentAnimIndex = iAnimIndex;
		m_isLoop = isLoop;
	}

	// Get
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_bool isFinished() {
		return m_Animations[m_iCurrentAnimIndex]->isFinished();
	}

	// File
	MODELFILE* Get_ModelFile() { Ready_ModelFile(); return &m_tModelFile; }

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);
	HRESULT			Initialize_Prototype(TYPE eType, MODELFILE* pModelFile/*const _char* strDataFilePath*/);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT	Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);
	HRESULT Play_Animation(_float fTimeDelta);
	HRESULT Render(_uint iMeshIndex);

	_bool	Check_Picking(const class CTransform* pTransform) const;

private: 
	// Assimp
	TYPE						m_eModelType = { TYPE_END };
	const aiScene*				m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;
	//_char						m_szName[MAX_PATH] = { "" };
	/*MODELFILE*					m_pModelFile;*/
	MODELFILE					m_tModelFile;
	vector<MATERIALFILE>		m_MaterialFiles;

private: 
	// Mesh	
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

	// Material
	_uint						m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>		m_Materials;

	// Bone
	_float4x4					m_TransformMatrix;
	vector<class CBone*>		m_Bones;

	// Animation
	_uint						m_iNumAnimations = { 0 };
	_uint						m_iCurrentAnimIndex = { 0 };
	_bool						m_isLoop = { false };
	vector<class CAnimation*>	m_Animations;

	_float4x4					m_MeshBoneMatrices[512];

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(aiNode* pAINode, _int iParentIndex = -1);
	HRESULT Ready_Animations();
	HRESULT Ready_ModelFile();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

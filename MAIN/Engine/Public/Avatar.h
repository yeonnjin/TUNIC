#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameInstance;
class CAnimation;
class CBone;
class CShader;

class CAvatar final : public CBase
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CAvatar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAvatar(const CAvatar& rhs);
	virtual ~CAvatar() = default;

public:
	vector<CAnimation*>*		Get_Animations_Ptr() { return &m_Animations; }
	vector<CBone*>*				Get_Bones_Ptr() { return &m_Bones; }
	CBone*						Get_Bone_Ptr(_uint iBoneIndex) const;

	_uint						Get_NumMeshes() const { return m_iNumMeshes;}


public:
	HRESULT Initialize_Prototype(TYPE eType, MODELFILE* pModelFile);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT	Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, AITEXTURETYPE eTextureType);
	HRESULT Render(_uint iMeshIndex);

private:
	TYPE							m_eModelType = { TYPE_END };
	// Mesh	
	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh*>			m_Meshes;

	// Material
	_uint							m_iNumMaterials = { 0 };
	vector<_uint>					m_NumTextures;
	vector<MESH_MATERIAL>			m_Materials;

	// Bone
	_float4x4						m_TransformationMatrix;
	vector<CBone*>					m_Bones;

	// Animation
	_uint							m_iNumAnimations = { 0 };
	_uint							m_iCurrentAnimIndex = { 0 };
	_bool							m_isLoop = { false };
	vector<CAnimation*>				m_Animations;

	_float4x4						m_MeshBoneMatrices[512];

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	CGameInstance*					m_pGameInstance = { nullptr };

	_bool							m_isCloned = { false };

private:
	HRESULT Ready_Meshes(_uint iNumMeshes, vector<MESHFILE>& pMeshFile);
	HRESULT Ready_Materials(_uint iNumMaterials, vector<_uint>& pNumTextures, vector<vector<MATERIALFILE>>& pMaterialFile);
	HRESULT Ready_Bones(_uint iNumBones, vector<BONEFILE>& pBoneFile);
	HRESULT Ready_Animations(_uint iNumAnimations, vector<ANIMFILE>& pAnimFile);

public:
	static CAvatar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, MODELFILE* pModelFile);
	virtual CAvatar* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
#pragma once

#include "Base.h"

/* 뼈의 정보를 표현하기 위한 세가지 타입 제공 */
/* aiNode, aiBone, aiAnimNode */

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	// Set
	void Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}

	void Set_CombinedTransformationPosition(_fvector vPosition) {
		memcpy(&m_CombinedTransformationMatrix.m[3], &vPosition, sizeof(_float3));
	} // Root

	void Set_ParentBoneIndex(_int iIndex) { m_iCustomBoneIndex = iIndex; }
	void Set_isUseTransformaion(_bool isUse) { m_isUse = isUse; }

	// Get
	const _float4x4* Get_CombinedTransformationMatrix() const {
		return &m_CombinedTransformationMatrix;
	}

	_float4x4 Get_TransformationMatrix() { return m_TransformationMatrix; } // Blending

	_float4	Get_Position();
	
public:
	HRESULT Initialize(BONEFILE* pBoneFile);
	void	Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix TransformationMatrix);
	_bool	Compare_Name(const _char* pBoneName) {
		return !strcmp(m_szName, pBoneName);
	}

private:
	_char			m_szName[MAX_PATH] = { "" };

	_float4x4		m_TransformationMatrix;				// 이 Bone 자체의 상태 행렬
	_float4x4		m_CombinedTransformationMatrix;		// 최종 상태 행렬

	_int			m_iParentBoneIndex = { -1 };
	_int			m_iCustomBoneIndex = { 2 };

	_bool			m_isUse = { false };

public:
	static CBone* Create(BONEFILE* pBoneFile);
	CBone* Clone();
	virtual void Free() override;
};

END
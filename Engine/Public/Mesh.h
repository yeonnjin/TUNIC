#pragma once

#include "VIBuffer.h"

/* 디자이너 분들이 저장해준 정점과 인덱스의 정보를 바탕으로 정점, 인덱스 버퍼 생성 */

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint	Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh);
	virtual HRESULT Initialize(void* pArg) override;

private:
	_char	m_szName[MAX_PATH] = { "" };
	_uint	m_iMaterialIndex = { 0 };		// 이 메쉬는 모델에서 로드 해놓은 머테리얼들 중 몇 번째 머테리얼을 이용하는가

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END
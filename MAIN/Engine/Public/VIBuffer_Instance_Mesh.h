#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance_Mesh final : public CVIBuffer_Instance
{
private:
	CVIBuffer_Instance_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance_Mesh(const CVIBuffer_Instance_Mesh& rhs);
	virtual ~CVIBuffer_Instance_Mesh() = default;

public:
	virtual HRESULT Initialize_Prototype(MESHFILE* pMeshFile, const CVIBuffer_Instance::INSTANCE_DESC& tInstanceDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual _float4 Compute_Random_Position() override;

private:
	_uint				m_iMaterialIndex = { 0 };
	_uint				m_iNumFaces = { 0 };

public:
	static CVIBuffer_Instance_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MESHFILE* pMeshFile, const CVIBuffer_Instance::INSTANCE_DESC& tInstanceDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END

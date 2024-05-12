#include "VIBuffer_Instance_Mesh.h"

CVIBuffer_Instance_Mesh::CVIBuffer_Instance_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Instance_Mesh::CVIBuffer_Instance_Mesh(const CVIBuffer_Instance_Mesh& rhs)
    : CVIBuffer_Instance{ rhs }
{
}

HRESULT CVIBuffer_Instance_Mesh::Initialize_Prototype(MESHFILE* pMeshFile, const CVIBuffer_Instance::INSTANCE_DESC& tInstanceDesc)
{
	if (FAILED(__super::Initialize_Prototype(tInstanceDesc)))
		return E_FAIL;

	m_iNumInstance = tInstanceDesc.iNumInstance;
	m_iInstanceStride = sizeof(VTXMATRIX);
	m_iIndexCountPerInstance = 6;

	m_iMaterialIndex = pMeshFile->iMaterialIndex;
	m_iNumVertices = pMeshFile->iNumVertices;

	m_pVerticesPos = new _float3[m_iNumVertices];

	m_iNumFaces = pMeshFile->iNumFaces;
	m_iNumIndices = m_iNumFaces * 3;

	m_iIndexStride = sizeof(_uint);
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점 버퍼의 byte 크기 */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pMeshFile->pMeshVertices[i].vPosition, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), XMMatrixIdentity()));
		m_pVerticesPos[i] = pVertices[i].vPosition;

		memcpy(&pVertices[i].vNormal, &pMeshFile->pMeshVertices[i].vNormal, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), XMMatrixIdentity()));

		memcpy(&pVertices[i].vTexcoord, &pMeshFile->pMeshVertices[i].vTexcoord, sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pMeshFile->pMeshVertices[i].vTangent, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), XMMatrixIdentity()));
	}

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 인덱스 버퍼의 byte 크기 */
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumIndices; ++i)
	{
		pIndices[i] = pMeshFile->pIndices[i];
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER

	ZeroMemory(&m_InstanceBufferDesc, sizeof(m_InstanceBufferDesc));

	/* 랜덤 설정 */
	uniform_real_distribution<_float>	ScaleX(tInstanceDesc.vMinScale.x, tInstanceDesc.vMaxScale.x);
	uniform_real_distribution<_float>	ScaleY(tInstanceDesc.vMinScale.y, tInstanceDesc.vMaxScale.y);
	uniform_real_distribution<_float>	ScaleZ(tInstanceDesc.vMinScale.z, tInstanceDesc.vMaxScale.z);

	uniform_real_distribution<_float>	LifeTime(tInstanceDesc.vLifeTime.x, tInstanceDesc.vLifeTime.y);
	uniform_real_distribution<_float>	Speed(tInstanceDesc.vSpeed.x, tInstanceDesc.vSpeed.y);

	/* 인스턴스 버퍼의 byte 크기 */
	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;	// 동적
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	m_pInstanceVertices = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXMATRIX) * m_iNumInstance);

	m_pLifeTimes = new _float2[m_iNumInstance];
	ZeroMemory(m_pLifeTimes, sizeof(_float2) * m_iNumInstance);

	m_pSpeeds = new _float[m_iNumInstance];
	ZeroMemory(m_pSpeeds, sizeof(_float) * m_iNumInstance);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		XMStoreFloat4(&m_pInstanceVertices[i].vRight, XMVectorSet(1.f, 0.f, 0.f, 0.f) * ScaleX(m_RandomNumber));
		XMStoreFloat4(&m_pInstanceVertices[i].vUp, XMVectorSet(0.f, 1.f, 0.f, 0.f) * ScaleY(m_RandomNumber));
		XMStoreFloat4(&m_pInstanceVertices[i].vLook, XMVectorSet(0.f, 0.f, 1.f, 0.f) * ScaleZ(m_RandomNumber));
		m_pInstanceVertices[i].vPosition = Compute_Random_Position();

		m_pInstanceVertices[i].isLived = true;

		/* 인스턴스마다 랜덤하게 설정된 각각의 라이프 타임 */
		m_pLifeTimes[i] = _float2(0.f, LifeTime(m_RandomNumber));
		m_pSpeeds[i] = Speed(m_RandomNumber);
	}

	ZeroMemory(&m_InstanceSubResourceData, sizeof(m_InstanceSubResourceData));
	m_InstanceSubResourceData.pSysMem = m_pInstanceVertices;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Instance_Mesh::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_float4 CVIBuffer_Instance_Mesh::Compute_Random_Position()
{
	uniform_real_distribution<_float>	RangePosX(m_InstanceDesc.vCenter.x - m_InstanceDesc.vRange.x * 0.5f, m_InstanceDesc.vCenter.x + m_InstanceDesc.vRange.x * 0.5f);
	uniform_real_distribution<_float>	RangePosY(m_InstanceDesc.vCenter.y - m_InstanceDesc.vRange.y * 0.5f, m_InstanceDesc.vCenter.y + m_InstanceDesc.vRange.y * 0.5f);
	uniform_real_distribution<_float>	RangePosZ(m_InstanceDesc.vCenter.z - m_InstanceDesc.vRange.z * 0.5f, m_InstanceDesc.vCenter.z + m_InstanceDesc.vRange.z * 0.5f);

	return _float4(RangePosX(m_RandomNumber), RangePosY(m_RandomNumber), RangePosZ(m_RandomNumber), 1.f);
}

CVIBuffer_Instance_Mesh* CVIBuffer_Instance_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MESHFILE* pMeshFile, const CVIBuffer_Instance::INSTANCE_DESC& tInstanceDesc)
{
	CVIBuffer_Instance_Mesh* pInstance = new CVIBuffer_Instance_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pMeshFile, tInstanceDesc)))
	{
		MSG_BOX(TEXT("Failed To Create : CVIBuffer_Instance_Mesh"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Instance_Mesh::Clone(void* pArg)
{
	CComponent* pInstance = new CVIBuffer_Instance_Mesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CVIBuffer_Instance_Mesh"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Instance_Mesh::Free()
{
	__super::Free();
}

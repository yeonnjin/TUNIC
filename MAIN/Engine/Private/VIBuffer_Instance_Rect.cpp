#include "VIBuffer_Instance_Rect.h"

CVIBuffer_Instance_Rect::CVIBuffer_Instance_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Instance_Rect::CVIBuffer_Instance_Rect(const CVIBuffer_Instance_Rect& rhs)
    : CVIBuffer_Instance{ rhs }
{
}

HRESULT CVIBuffer_Instance_Rect::Initialize_Prototype(const CVIBuffer_Instance::INSTANCE_DESC& tInstanceDesc)
{
    m_iNumInstance = tInstanceDesc.iNumInstance;
    m_iInstanceStride = sizeof(VTXMATRIX);
    m_iIndexCountPerInstance = 6;

    m_iNumVertices = 4;
    m_iVertexStride = sizeof(VTXPOSTEX);

    m_iNumIndices = m_iIndexCountPerInstance * m_iNumInstance;
    m_iIndexStride = sizeof(_ushort);
    m_iNumVertexBuffers = 2;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점 버퍼의 byte 크기 */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.0f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.0f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.0f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.0f, 1.f);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
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

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 1;
		pIndices[iNumIndices++] = 2;

		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 2;
		pIndices[iNumIndices++] = 3;
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
	uniform_real_distribution<_float>	RangePosX(tInstanceDesc.vPivot.x - tInstanceDesc.vRange.x * 0.5f, tInstanceDesc.vPivot.x + tInstanceDesc.vRange.x * 0.5f);
	uniform_real_distribution<_float>	RangePosY(tInstanceDesc.vPivot.y - tInstanceDesc.vRange.y * 0.5f, tInstanceDesc.vPivot.y + tInstanceDesc.vRange.y * 0.5f);
	uniform_real_distribution<_float>	RangePosZ(tInstanceDesc.vPivot.z - tInstanceDesc.vRange.z * 0.5f, tInstanceDesc.vPivot.z + tInstanceDesc.vRange.z * 0.5f);

	uniform_real_distribution<_float>	ScaleX(tInstanceDesc.vMinScale.x, tInstanceDesc.vMaxScale.x);
	uniform_real_distribution<_float>	ScaleY(tInstanceDesc.vMinScale.y, tInstanceDesc.vMaxScale.y);
	uniform_real_distribution<_float>	ScaleZ(tInstanceDesc.vMinScale.z, tInstanceDesc.vMaxScale.z);

	uniform_real_distribution<_float>	LifeTime(tInstanceDesc.vLifeTime.x, tInstanceDesc.vLifeTime.y);

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

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		XMStoreFloat4(&m_pInstanceVertices[i].vRight, XMVectorSet(1.f, 0.f, 0.f, 0.f) * ScaleX(m_RandomNumber));
		XMStoreFloat4(&m_pInstanceVertices[i].vUp, XMVectorSet(0.f, 1.f, 0.f, 0.f) * ScaleY(m_RandomNumber));
		XMStoreFloat4(&m_pInstanceVertices[i].vLook, XMVectorSet(0.f, 0.f, 1.f, 0.f) * ScaleZ(m_RandomNumber));
		m_pInstanceVertices[i].vPosition = _float4(RangePosX(m_RandomNumber), RangePosY(m_RandomNumber), RangePosZ(m_RandomNumber), 1.f);
	
		m_pInstanceVertices[i].isLived = true;

		/* 인스턴스마다 랜덤하게 설정된 각각의 라이프 타임 */
		m_pLifeTimes[i] = _float2(0.f, LifeTime(m_RandomNumber));
	}

	ZeroMemory(&m_InstanceSubResourceData, sizeof(m_InstanceSubResourceData));
	m_InstanceSubResourceData.pSysMem = m_pInstanceVertices;

#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Instance_Rect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    return S_OK;
}

CVIBuffer_Instance_Rect* CVIBuffer_Instance_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instance::INSTANCE_DESC& tInstanceDesc)
{
	CVIBuffer_Instance_Rect* pInstance = new CVIBuffer_Instance_Rect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(tInstanceDesc)))
	{
		MSG_BOX(TEXT("Failed To Create : CVIBuffer_Instance_Rect"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Instance_Rect::Clone(void* pArg)
{
	CComponent* pInstance = new CVIBuffer_Instance_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CVIBuffer_Instance_Rect"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Instance_Rect::Free()
{
	__super::Free();
}

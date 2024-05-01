#include "Cell.h"

#ifdef _DEBUG
#include "VIBuffer_Cell.h"
#endif // _DEBUG

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CCell::Set_NeighborIndex(_int* pNeighborindex)
{
	for (size_t i = 0; i < 3; i++)
		m_iNeighborIndices[i] = pNeighborindex[i];
}

_bool CCell::isIn(_fvector vPosition, _fmatrix TerrainWorldMatrix, _int* pNeighborIndex)
{
	_vector		vPoints[3];

	for (size_t i = 0; i < POINT_END; i++)
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), TerrainWorldMatrix);

	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector		vSourDir = XMVector3Normalize(vPosition - vPoints[i]);

		_vector		vDir = vPoints[i + 1 >= POINT_END ? 0 : i + 1] - vPoints[i];
		_vector		vDestDir = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vDir) * -1.f, 0.f, XMVectorGetX(vDir), 0.0f));

		_vector vDot = XMVector3Dot(vSourDir, vDestDir);
		if (0 < XMVectorGetX(XMVector3Dot(vSourDir, vDestDir)))
		{
			*pNeighborIndex = m_iNeighborIndices[i];
			return false;
		}
	}

	return true;

	//_vector	vPoints[3];
	//_vector vObjectPosition = vPosition;
	//vObjectPosition.m128_f32[1] = 0.f;

	//// 월드 위치 변환
	//for (size_t i = 0; i < POINT_END; i++)
	//{
	//	vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), TerrainWorldMatrix);
	//	vPoints[i].m128_f32[1] = 0.f;
	//}

	//for (size_t i = 0; i < LINE_END; i++)
	//{
	//	// 첫 번째 점 - 오브젝트 위치 (정규화)
	//	_vector	vSrcDir = XMVector3Normalize(vObjectPosition - vPoints[i]);

	//	// 첫 번째 점 - 두 번째 점
	//	_vector	vTargetDir = vPoints[(i + 1) % 3] - vPoints[i];
	//	
	//	// 수직 벡터 (정규화)
	//	_vector vDstDir = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vTargetDir) * -1, 0.f, XMVectorGetX(vTargetDir), 0.f));

	//	// 외적 : 해당 방향으로 나갔으면 이웃 인덱스 갱신 (이동한 인덱스)
	//	if (0 < XMVectorGetX(XMVector3Dot(vSrcDir, vDstDir)))
	//	{
	//		*pNeighborIndex = m_iNeighborIndices[i];
	//		return false;
	//	}
	//}

	//return true;
}

_bool CCell::Compare_Points(_fvector vSrcPoint, _fvector vDstPoint)
{
	for (size_t i = 0; i < POINT_END; ++i)
	{
		if (true == XMVector3Equal(vSrcPoint, XMLoadFloat3(&m_vPoints[i])))
		{
			if (true == XMVector3Equal(vDstPoint, XMLoadFloat3(&m_vPoints[(i + 1) % 3])))
				return true;

			if (true == XMVector3Equal(vDstPoint, XMLoadFloat3(&m_vPoints[(i + 2) % 3])))
				return true;
		}
	}
	
	return false;
}

_float CCell::Compute_Height(_fvector vPosition, _fmatrix TerrainWorldMatrix)
{
	_vector		vPoints[3];

	for (size_t i = 0; i < POINT_END; i++)
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), TerrainWorldMatrix);

	 _vector vPlane = XMPlaneFromPoints(vPoints[0], vPoints[1], vPoints[2]);

	 /* ax + by + cz + d = 0 */
	/* y = (-ax - cz - d) / b*/
	 return (-vPlane.m128_f32[0] * vPosition.m128_f32[0] - vPlane.m128_f32[2] * vPosition.m128_f32[2] - vPlane.m128_f32[3]) / vPlane.m128_f32[1];
}

HRESULT CCell::Initialize(const _float3* pPoints, _uint iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	m_iIndex = iIndex;

#ifdef _DEBUG
	m_pVIBufferCom = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
	m_pVIBufferCom->Bind_Buffers();

	return m_pVIBufferCom->Render();
}
#endif // _DEBUG

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX(TEXT("Failed To Create : CCell"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	__super::Free();
#ifdef _DEBUG
	Safe_Release(m_pVIBufferCom);
#endif

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

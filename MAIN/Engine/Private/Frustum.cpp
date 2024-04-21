#include "Frustum.h"

#include "GameInstance.h"

CFrustum::CFrustum()
    : m_pGameInstance{ CGameInstance::Get_Instance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
	// 2 * 2 * 1
	m_vOriginPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vOriginPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vOriginPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vOriginPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vOriginPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vOriginPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vOriginPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vOriginPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Tick()
{
	_matrix			ViewMatrixInv, ProjMatrixInv;

	ViewMatrixInv = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
	ProjMatrixInv = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);

	_vector			vPoints[8];

	for (size_t i = 0; i < 8; i++)
	{
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vOriginPoints[i]), ProjMatrixInv);
		XMStoreFloat3(&m_vWorldPoints[i], XMVector3TransformCoord(vPoints[i], ViewMatrixInv));
	}

	Make_Planes(m_vWorldPoints, m_vWorldPlanes);
}

_bool CFrustum::isIn_WorldSpace(_fvector vWorldPos, _float fRange)
{
	for (size_t i = 0; i < 6; i++)
	{
		/*a b c d
		x y z 1
		ax + by + cz + d * 1 < 0;*/

		// ÀÔ·Â Æò¸é°ú 3D º¤ÅÍ »çÀÌÀÇ Á¡ °öÀ» °è»ê
		// vectorOut.x = P.x * V.x + P.y * V.y + P.z * V.z + P.w * 1.0f;
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vWorldPlanes[i]), vWorldPos))) 
			return false;																		 
	}

	return true;
}

void CFrustum::Make_Planes(const _float3* pPoints, _float4* pPlanes)
{
	// ¹Ù±ùÂÊ
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[6]))); // ¿À¸¥ÂÊ
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[3]))); // ¿ÞÂÊ

	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[1]))); // À­¸é
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat3(&pPoints[3]), XMLoadFloat3(&pPoints[2]), XMLoadFloat3(&pPoints[6]))); // ¾Æ·§¸é

	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[7]))); // µÞ¸é
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[2]))); // ¾Õ¸é
}

CFrustum* CFrustum::Create()
{
	CFrustum* pInstance = new CFrustum();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CFrustum"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrustum::Free()
{
	Safe_Release(m_pGameInstance);
}



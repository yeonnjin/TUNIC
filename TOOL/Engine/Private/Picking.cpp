#include "Picking.h"
#include "GameInstance.h"

CPicking::CPicking()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	m_hWnd = hWnd;

	m_iWinSizeX = iWinSizeX;
	m_iWinSizeY = iWinSizeY;

	return S_OK;
}

void CPicking::Update()
{
	// 마우스 위치
	POINT		ptMouse = {};
	GetCursorPos(&ptMouse);

	// 뷰포트 상 마우스 위치
	ScreenToClient(m_hWnd, &ptMouse);

	// 투영 스페이스 상의 마우스 위치
	_float3		vMousePos;
	vMousePos.x = ptMouse.x / (m_iWinSizeX * 0.5f) - 1.f;
	vMousePos.y = ptMouse.y / -(m_iWinSizeY * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	// 뷰 스페이스 상의 마우스 위치
	XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ)));

	// 뷰 스페이스 상의 마우스 레이 + 레이의 시작점 구하기
	m_vRayDir = /*vMousePos;*/ _float3(vMousePos.x - 0.f, vMousePos.y - 0.f, vMousePos.z - 0.f);
	m_vRayPos = _float3(0.f, 0.f, 0.f);

	// 월드 스페이스 상의 마우스 레이 + 레이의 시작점
	_matrix ViewMatrixInv = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
	XMStoreFloat3(&m_vRayDir, XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), ViewMatrixInv));
	XMStoreFloat3(&m_vRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), ViewMatrixInv));
}

void CPicking::Transform_Picking_To_LocalSpace(const CTransform* pTransform, _Out_ _float3* pRayDir, _Out_ _float3* pRayPos)
{
	_matrix WorldMatrixInv = pTransform->Get_WorldMatrix_Inverse();

	XMStoreFloat3(pRayDir, XMVector3TransformNormal(XMLoadFloat3(&m_vRayDir), WorldMatrixInv));
	XMStoreFloat3(pRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vRayPos), WorldMatrixInv));
}

CPicking* CPicking::Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	CPicking* pInstance = new CPicking();

	if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
	{
		MSG_BOX(TEXT("Failed To Create : CPicking"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPicking::Free()
{
	Safe_Release(m_pGameInstance);
}

#include "Navigation.h"

#include "Cell.h"
#include "Shader.h"
#include "GameInstance.h"

_float4x4   CNavigation::m_WorldMatrix{};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& rhs)
    : CComponent{ rhs }
    , m_Cells{ rhs.m_Cells }
#ifdef _DEBUG
    , m_pShader{ rhs.m_pShader }
#endif // _DEBUG
{
#ifdef _DEBUG
    Safe_AddRef(m_pShader);
#endif

    for (auto& pCell : m_Cells)
        Safe_AddRef(pCell);
}

HRESULT CNavigation::Initialize_Prototype(const wstring& strDataFile)
{
	_ulong		dwByte = { 0 };
	HANDLE		hFile = CreateFile(strDataFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3		vPoints[3];

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.emplace_back(pCell);
	}

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr != pArg)
		m_iCurrentIndex = ((NAVIGATION_DESC*)pArg)->iCurrentIndex;

    return S_OK;
}

void CNavigation::Tick(_fmatrix WorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

_bool CNavigation::isMove(_fvector vPosition)
{
	if (-1 == m_iCurrentIndex)
		return false;

	_int	iNeighborIndex = { -1 };

	// 현재 셀에 그대로 있을 때
	if (true == m_Cells[m_iCurrentIndex]->isIn(vPosition, XMLoadFloat4x4(&m_WorldMatrix), &iNeighborIndex))
		return true;
	// 밖에 나갔을 때
	else
	{
		// 나간 방향에 이웃이 없었을 때
		if (-1 == iNeighborIndex)
			return false;
		// 나간 방향에 이웃이 있었을 때
		else
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, XMLoadFloat4x4(&m_WorldMatrix), &iNeighborIndex))
				{
					m_iCurrentIndex = iNeighborIndex;
					return true;
				}
			}
		}		
	}
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	m_pShader->Begin(0);

	if (-1 == m_iCurrentIndex)
	{
		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
				pCell->Render();
		}
	}
	else
		m_Cells[m_iCurrentIndex]->Render();

	return S_OK;
}
#endif

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSrcCell : m_Cells)
	{
		for (auto& pDstCell : m_Cells)
		{
			if (pSrcCell == pDstCell)
				continue;

			if (true == pDstCell->Compare_Points(pSrcCell->Get_Point(CCell::POINT_A), pSrcCell->Get_Point(CCell::POINT_B)))
				pSrcCell->SetUp_Neighbor(CCell::LINE_AB, pDstCell);

			if (true == pDstCell->Compare_Points(pSrcCell->Get_Point(CCell::POINT_B), pSrcCell->Get_Point(CCell::POINT_C)))
				pSrcCell->SetUp_Neighbor(CCell::LINE_BC, pDstCell);

			if (true == pDstCell->Compare_Points(pSrcCell->Get_Point(CCell::POINT_C), pSrcCell->Get_Point(CCell::POINT_A)))
				pSrcCell->SetUp_Neighbor(CCell::LINE_CA, pDstCell);
		}
	}

	return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strDataFile)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strDataFile)))
	{
		MSG_BOX(TEXT("Failed To Create : CNavigation"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CNavigation"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif
}

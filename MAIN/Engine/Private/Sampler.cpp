#include "Sampler.h"
#include "GameInstance.h"

CSampler::CSampler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CSampler::Initialize()
{
	_uint			iNumViewports = { 1 };
	D3D11_VIEWPORT	ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_iViewportWidth = ViewportDesc.Width;
	m_iViewportHeight = ViewportDesc.Height;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = ViewportDesc.Width;
	TextureDesc.Height = ViewportDesc.Height;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	// DepthStencil

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_STAGING;				// 읽기 전용
	TextureDesc.BindFlags = 0;								// 용도 지정 : 저장만 하고 따로 변환해서 쓰지X
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;		// STAGING 옵션 덕분에 데이터 읽기 가능
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pRTTexture)))
		return E_FAIL;

	return S_OK;
}

_vector CSampler::Compute_WorldPos(const _float2& vViewportPos, const wstring& strZRenderTargetTag, _uint iOffset)
{
	if (FAILED(m_pGameInstance->Copy_Resource(strZRenderTargetTag, &m_pRTTexture)))
		return XMVectorZero();

	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pRTTexture, 0, D3D11_MAP_READ, 0, &SubResource);

	// 루프를 돌지 않아도 한 번에 해당 픽셀 인덱스 특정 가능
	_uint	iPixelindex = vViewportPos.y * m_iViewportWidth + vViewportPos.x;

	// 화면 상의 픽셀 위치를 받아 인덱스로 계산한 후 1바이트로 캐스팅
	_ubyte* pAddress = ((_ubyte*)(((_float4*)SubResource.pData) + iPixelindex));

	// 
	_float	fZ = *((_float*)(pAddress + iOffset));

	m_pContext->Unmap(m_pRTTexture, 0);

	_float3	vProjPos;

	// PROJ : -1 ~ 1 사이로 변환
	vProjPos.x = vViewportPos.x / (m_iViewportWidth * 0.5f) - 1.f;
	vProjPos.y = vViewportPos.y / -(m_iViewportHeight * 0.5f) + 1.f;
	vProjPos.z = fZ;

	// WORLD 로 변환
	_matrix	ProjMatrixInv = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);
	_matrix ViewMatrixInv = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);

	_vector	vWorldPos = XMVector3TransformCoord(XMLoadFloat3(&vProjPos), ProjMatrixInv);
	vWorldPos = XMVector3TransformCoord(vWorldPos, ViewMatrixInv);

	return vWorldPos;
}

CSampler* CSampler::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSampler* pInstance = new CSampler(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CSampler"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSampler::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRTTexture);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

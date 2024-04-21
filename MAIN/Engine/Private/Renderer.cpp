#include "Renderer.h"
#include "GameObject.h"

#include "GameInstance.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance {CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	// 뷰포트 정보를 받아서 Width, Height 세팅
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	_uint iWidth = ViewportDesc.Width;
	_uint iHeight = ViewportDesc.Height;
	/* 디퍼드 셰이딩을 위한 렌더 타켓들을 생성 */

	/* For. Target_Diffuse */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For. Target_Normal */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;		// UNORM : 0 ~ 1 사이 변환

	/* For. Target_Depth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;		// Depth 기록 시 16비트는 소수점 이하가 짤려 정밀도가 떨어지므로 32비트로 저장, y가 0.f 일 경우 하늘과 색이 섞일 때 값이 0 이므로 그려지지 않음
	
	/* For.Target_FieldDepth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_FieldDepth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* For. Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	
	/* For. Target_Specular */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* MRT_GameObjects : 객체들의 특정 정보를 받아오기 위한 렌더 타겟들 */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_FieldDepth"))))
		return E_FAIL;

	/* MRT_LightAcc : 빛들의 연산 결과 정보를 받아오기 위한 렌더 타겟 */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;		// 객체 정보들을 다 받은 후 빛 연산을 위한 셰이드 타겟
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	// 화면 전체를 덮는 VIBuffer_Rect ( 직교 투영 )
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	m_WorldMatrix._41 = 0.f;
	m_WorldMatrix._42 = 0.f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

#ifdef  _DEBUG
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Diffuse"), 100.0f, 100.0f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Normal"), 100.0f, 300.0f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Depth"), 100.0f, 500.0f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Shade"), 300.0f, 100.0f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Specular"), 300.0f, 300.0f, 200.f, 200.f)))
		return E_FAIL;
#endif //  _DEBUG

	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pRenderObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Render()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_NonBlend()))
		return E_FAIL;

	if (FAILED(Render_Lights()))
		return E_FAIL;

	if (FAILED(Render_Result()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Render_Debug()))
		return E_FAIL;
#endif

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pRenderComponent)
{
	m_DebugComponents.emplace_back(pRenderComponent);

	Safe_AddRef(pRenderComponent);

	return S_OK;
}
#endif // _DEBUG

HRESULT CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if(nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	/* 0. 렌더 타겟을 교체 */
	/* 1. 이 그룹에 있는 객체들은 다 빛 연산이 필요하다고 가정 => 빛 연산을 후처리로 처리할 것이다 */
	/* 2. 후처리를 위해서는 빛 연산을 위한 데이터가 필요 => 빛 : 빛 매니저, ☆노멀,재질☆ : 이걸 받아오기 위해 렌더 타겟에 저장해서 받아옴 */
	/* 3. Diffuse를 0번째, Normal을 1번째에 세팅 => 순서대로 함수 호출 필요 */

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_FIELD])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_FIELD].clear();

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_NONBLEND].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}
HRESULT CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}


//_bool Compare(CGameObject* pSour, CGameObject* pDest)
//{
//	return ((CGameObject*)pSour)->Get_ViewZ() > ((CGameObject*)pDest)->Get_ViewZ();
//}

HRESULT CRenderer::Render_Blend()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	//m_RenderObjects[RENDER_UI].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	//	{
	//		return ((CGameObject*)pSour)->Get_ViewZ() > ((CGameObject*)pDest)->Get_ViewZ();
	//	});

	for (auto& pRenderObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	/* 매트릭스 세팅 */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;


	/* 카메라 세팅 */
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	_float fCamFar = m_pGameInstance->Get_Camera_Far();
	if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	/* 렌더 타겟 적용 */
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;


	/* 버퍼 바인드 */
	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;


	/* SHADE */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Result()
{
	/* 백 버퍼에 디퍼드 방식으로 연산된 최종 결과물을 찍어줌 */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Shade"), "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Specular"), "g_SpecularTexture")))
		return E_FAIL;

	m_pShader->Begin(3);	// pass Final

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugComponents)
	{
		if (nullptr != pDebugCom)
			pDebugCom->Render();

		Safe_Release(pDebugCom);
	}
	m_DebugComponents.clear();

	/*if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
		return E_FAIL;*/

	return S_OK;
}
#endif

CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer*		pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Create : CRenderer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CRenderer::Free()
{
	__super::Free();

	for (auto& RenderList : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderList)		
			Safe_Release(pRenderObject);
		RenderList.clear();		
	}

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

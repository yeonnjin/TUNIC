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
	//_uint			iNumViewports = { 1 };
	//D3D11_VIEWPORT	ViewportDesc = {};

	//// 뷰포트 정보를 받아서 Width, Height 세팅
	//m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	///* 디퍼드 셰이딩을 위한 렌더 타켓들을 생성 */

	///* For. Target_Diffuse */
	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
	//	return E_FAIL;

	///* For. Target_Normal */
	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
	//	return E_FAIL;		// UNORM : 0 ~ 1 사이 변환

	///* MRT_GameObjects : 객체들의 특정 정보를 받아오기 위한 렌더 타겟들 */
	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
	//	return E_FAIL;

//	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
//		return E_FAIL;		// 객체 정보들을 다 받은 후 빛 연산을 위한 셰이드 타겟
//
//	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
//	if (nullptr == m_pVIBuffer)
//		return E_FAIL;
//
//	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
//	if (nullptr == m_pShader)
//		return E_FAIL;
//
//	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
//
//	// 화면 전체를 덮는 VIBuffer_Rect ( 직교 투영 )
//	m_WorldMatrix._11 = ViewportDesc.Width;
//	m_WorldMatrix._22 = ViewportDesc.Height;
//	m_WorldMatrix._41 = 0.f;
//	m_WorldMatrix._42 = 0.f;
//
//	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
//	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));
//
//#ifdef  _DEBUG
//	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Diffuse"), 150.0f, 150.0f, 300.f, 300.f)))
//		return E_FAIL;
//	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Normal"), 150.0f, 450.0f, 300.f, 300.f)))
//		return E_FAIL;
//	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Shade"), 450.0f, 150.0f, 300.f, 300.f)))
//		return E_FAIL;
//#endif //  _DEBUG

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

	/*if (FAILED(Render_Lights()))
		return E_FAIL;*/

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

//#ifdef _DEBUG
//	if (FAILED(Render_Debug()))
//		return E_FAIL;
//#endif

	return S_OK;
}

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

	/*if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;*/

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_NONBLEND].clear();

	/*if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;*/

	return S_OK;
}
//
//_bool Compare(CGameObject* pSour, CGameObject* pDest)
//{
//	return ((CBlendObject*)pSour)->Get_ViewZ() > ((CBlendObject*)pDest)->Get_ViewZ();
//}

HRESULT CRenderer::Render_Blend()
{
	//m_RenderObjects[RENDER_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	//{
	//	return ((CBlendObject*)pSour)->Get_ViewZ() > ((CBlendObject*)pDest)->Get_ViewZ();
	//});

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

	/* 노말 텍스쳐 적용 */
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
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

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

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

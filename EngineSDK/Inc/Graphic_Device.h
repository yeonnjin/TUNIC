#pragma once

#include "Base.h"

BEGIN(Engine)

/* 1. ID3D11Device, ID3D11DeviceContext라는 객체를 우선 생성 */
/* 2. IDXGISwapChain를 생성 + (백버퍼(ID3D11Texture2D)도 같이 생성) */
/* 3. 백 버퍼 뷰를 생성 */
/* 4. 깊이 버퍼 텍스쳐를 생성하고 깊이 버퍼 뷰를 생성 */

class CGraphic_Device final : public CBase
{
public:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:

	/* 그래픽 디바이스 초기화 */
	HRESULT Ready_Graphic_Device(const ENGINE_DESC& EngineDesc, 
		_Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);

	/* 백 버퍼를 지우기 */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);

	/* 깊이버퍼 + 스텐실버퍼를 지우기 */
	HRESULT Clear_DepthStencil_View();

	/* 후면 버퍼를 전면버퍼로 교체 (백버퍼를 화면에 직접 보여줌) */
	HRESULT Present();

private:	
	// IDirect3DDevice9* == LPDIRECT3DDEVICE9 == ID3D11Device + ID3D11DeviceContext 

	/* 메모리 할당 (정점 버퍼, 인덱스 버퍼, 텍스쳐 로드), 컴객체의 생성과 관련된 역할 */
	ID3D11Device*				m_pDevice = { nullptr };
		
	/* 기능 실행 (바인딩 작업, 정점 버퍼를 SetStreamSource(), SetIndices(), SetTransform(), SetTexture()) */
	/* 그리기 */
	ID3D11DeviceContext*		m_pDeviceContext = { nullptr };

	/* 후면 버퍼와 전면 버퍼를 교체해가면서 화면에 보여주는 역할 */
	IDXGISwapChain*				m_pSwapChain = { nullptr };

	/* IDirect3DTexture9 */
	/* ID3D11Texture2D : 텍스쳐를 표현하는 사전 객체 타입
						 why? 용도에 맞는 실질적으로 사용하기 위한 텍스쳐 객체를 만들어내기 위함  */	

	/* ID3D11ShaderResourceView : 셰이더에 전달될 수 있는 텍스처 타입 */	
	/* ID3D11RenderTargetView : 렌더 타겟용으로 사용될 수 있는 텍스처 타입 */
	/* ID3D11DepthStencilView : 깊이스텐실 버퍼로서 사용될 수 있는 타입 */
	ID3D11RenderTargetView*		m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView*		m_pDepthStencilView = { nullptr };

private:
	/* 스왑 체인에게 필수적으로 필요한 데이터는 백 버퍼가 필요하므로 백 버퍼를 생성 하기 위한 정보를 던져줌 */
	/* 스왑 체인을 만들었다 == 백버퍼(텍스쳐)가 생성 */
	HRESULT Ready_SwapChain(HWND hWnd, _bool isWindowed, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

public:
	static CGraphic_Device* Create(const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice,
		_Out_ ID3D11DeviceContext** ppDeviceContextOut);
	virtual void Free() override;
};

END
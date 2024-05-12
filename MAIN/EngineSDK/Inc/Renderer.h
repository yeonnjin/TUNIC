#pragma once

#include "Base.h"

/* 1. 화면에 그려져야할 객체들만 그려지는 순서대로 보관하는 클래스이다.*/
/* 2. 보관하고 있는 순서대로 객체들의 Draw콜(렌더함수를호출한다.)을 수행한다.*/

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_SHADOW, RENDER_FIELD, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_UI, RENDER_FONT, RENDER_END };
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;
public:
	HRESULT Initialize();
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Render();

	void	Set_ShadowPosition(_vector vEye, _vector vLookAt) { m_vEye = vEye; m_vLookAt = vLookAt; }
	
#ifdef _DEBUG
	HRESULT Add_DebugComponent(class CComponent* pRenderComponent);
#endif // _DEBUG

public:
	_vector								m_vEye = { -50.f, 60.f, -171.f, 1.f };
	_vector								m_vLookAt = { -49.f, 59.f, -170.f, 1.f };
	
private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr };
	list<class CGameObject*>			m_RenderObjects[RENDER_END];

private:
	class CVIBuffer_Rect*				m_pVIBuffer = { nullptr };
	class CShader*						m_pShader = { nullptr };
	_float4x4							m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
	ID3D11DepthStencilView*				m_pLightDepthDSV = { nullptr };

#ifdef _DEBUG
private:
	list<class CComponent*>				m_DebugComponents;
#endif // _DEBUG

private:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonBlend();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_UI();
	HRESULT Render_Font();

private:
	HRESULT Render_Lights();
	HRESULT Render_Result();
	HRESULT Render_Bloom();

#ifdef _DEBUG
	HRESULT Render_Debug();
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
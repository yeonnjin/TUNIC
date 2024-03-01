#pragma once

#include "Base.h"

#include "imgui.h"

BEGIN(Engine)

class CImGui_Manager final: public CBase
{
private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	New_Frame();
	HRESULT Render();

	void	Set_Perspective(_bool isPerspective, _float fFov, _float fViewWidth);
	void	EditTransform(class CTransform* pTransformCom);

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	ImGuiIO m_io;

private:
	// GIZMO - Perspective
	void	Perspective(float fovyInDegrees, float aspectRatio, float znear, float zfar, float* m16);
	void	Frustum(float left, float right, float bottom, float top, float znear, float zfar, float* m16);
	void	OrthoGraphic(const float l, float r, float b, const float t, float zn, const float zf, float* m16);

public:
	static CImGui_Manager* Create(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
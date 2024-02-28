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
	void	EditTransform(_float* cameraView, _float* cameraProjection, _float* matrix, _bool editTransformDecomposition);

public:
	static CImGui_Manager* Create(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
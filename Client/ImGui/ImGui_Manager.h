#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "imgui.h"

BEGIN(Client)

class CImGui_Manager final: public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)
private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	New_Frame();
	HRESULT Render();
	void	EditTransform(_float* cameraView, _float* cameraProjection, _float* matrix, _bool editTransformDecomposition);

public:
	virtual void Free() override;
};

END
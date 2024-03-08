#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking();
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	void	Update();
	void	Transform_Picking_To_LocalSpace(const class CTransform* pTransform, _Out_ _float3* pRayDir, _Out_ _float3* pRayPos);

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	HWND					m_hWnd = {};
	_uint					m_iWinSizeX, m_iWinSizeY = {};

	_float3					m_vRayDir, m_vRayPos;

public:
	static CPicking* Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;
};

END
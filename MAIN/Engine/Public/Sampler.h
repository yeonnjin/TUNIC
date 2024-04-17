#pragma once

#include "Base.h"

/* 특정 Viewport 상의 위치 X, Y를 던져주면 해당 위치에 있는 픽셀의 월드 위치를 계산해줌 */
/* 깊이 렌더 타겟으로부터 Z 값을 얻어옴 */

BEGIN(Engine)

class CSampler final : public CBase
{
private:
	CSampler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSampler() = default;

public:
	HRESULT Initialize();

public:
	_vector Compute_WorldPos(const _float2& vViewportPos, const wstring& strZRenderTargetTag, _uint iOffset);

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

	_uint					m_iViewportWidth = { 0 };
	_uint					m_iViewportHeight = { 0 };

	/* 렌더 타겟용으로 만들어진 텍스쳐로부터 직접 값을 얻어올 수 없음 */
	/* 정보를 얻어오고 싶은 렌더 타겟의 정보를 복사 받아 놓기 위한 텍스쳐 */
	ID3D11Texture2D*		m_pRTTexture = { nullptr };

public:
	static CSampler* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
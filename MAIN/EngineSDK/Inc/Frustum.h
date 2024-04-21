#pragma once

#include "Base.h"

/* 1. 투영 공간 상의 절두체 8개 점을 정의 */
/* 2. Tick 함수에서 매 프레임마다 한 번씩 월드로 면을 역변환 */
/* 3. 절두체와 점의 비교를 수행하여 안에 있는지, 없는지 판단 */

BEGIN(Engine)

class CFrustum final : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT		Initialize();
	void		Tick();

	_bool		isIn_WorldSpace(_fvector vWorldPos, _float fRange);

private:
	_float3		m_vOriginPoints[8];
	_float3		m_vWorldPoints[8];
	_float4		m_vWorldPlanes[6];

private:
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	void Make_Planes(const _float3* pPoints, _float4* pPlanes);

public:
	static CFrustum* Create();
	virtual void Free() override;
};

END
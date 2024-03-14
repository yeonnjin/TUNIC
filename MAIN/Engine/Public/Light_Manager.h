#pragma once

#include "Base.h"

/* 후처리 셰이더에서 더 복잡하게 사용 */

BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);

public:
	HRESULT Initialize();
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);

private: // 컨테이너는 후에 필요하면 바꾸기 (이름과 함께 쓰려면 map으로 저장 -> 동적 푸쉬X)
	list<class CLight*>				m_Lights;

public:
	static CLight_Manager* Create();
	virtual void Free() override;
};

END
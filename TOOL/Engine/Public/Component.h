#pragma once

/* 컴포넌트의 공동적인 기능을 가짐  */
/* 모든 컴포넌트의 부모클래스를 정의함으로써 객체안에 컴포넌트들을 저장할때 하나의 타입(다형성)으로 저장, 관리할 수 있게 함 */

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

protected:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr };

	_bool								m_isCloned = { false };

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
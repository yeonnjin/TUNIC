#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CEditor final : public CGameObject
{
private:
	CEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEditor(const CEditor& rhs);
	virtual ~CEditor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	void Test();

private:
	bool show_demo_window = true;
	bool show_another_window = true;
	_float4 clear_color = _float4();

public:
	static CEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
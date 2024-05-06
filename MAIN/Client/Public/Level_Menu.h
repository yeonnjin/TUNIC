#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Menu final : public CLevel
{
private:
	CLevel_Menu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Menu() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_UI();
	HRESULT Ready_Layer_Player();
	HRESULT Ready_Layer_Camera();

public:
	static CLevel_Menu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
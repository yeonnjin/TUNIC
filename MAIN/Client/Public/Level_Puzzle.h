#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Puzzle final : public CLevel
{
private:
	CLevel_Puzzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Puzzle() = default;

public:
	void	Set_NextLevel(LEVEL eNextLevel);

public:
	virtual HRESULT Initialize() override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool	m_isNext = { false };
	LEVEL	m_eNextLevel = {};

private:
	HRESULT Ready_Layer_Map(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_UI();
	HRESULT Ready_Layer_Object(const wstring& strLayerTag);
	HRESULT Ready_Layer_Player();
	HRESULT	Ready_Layer_Camera();

public:
	static CLevel_Puzzle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END


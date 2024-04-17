#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CInventory final : public CBase
{
public:
	CInventory();
	virtual ~CInventory() = default;

public:
	void	Set_Using(_bool isUsing) { m_isUsing = isUsing; }

public:
	HRESULT Initialize();
	void	Tick(_float fTimeDelta);

private:
	_bool	m_isUsing = { false };

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	class CUI_Inventory*	m_pUIInventory = { nullptr };
	class CUI_Slot*			m_pUISlot = { nullptr };

public:
	static CInventory* Create();
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Item.h"

BEGIN(Client)

class CInventory final : public CBase
{
public:
	CInventory();
	virtual ~CInventory() = default;

public:
	void	Add_Item(class CItem* pItem);
	void	Select_Item();

	void	Set_Using(_bool isUsing) { m_isUsing = isUsing; }

public:
	HRESULT Initialize();
	void	Tick(_float fTimeDelta);

private:
	_bool	m_isUsing = { false };

	_uint	m_iMaxItem = { 4 };

	_uint	m_iSelectRow = { 0 };
	_uint	m_iSelectColumn = { 0 };

	vector<class CItem*>	m_Items[CItem::TYPE_END];

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	class CUI_Inventory*	m_pUIInventory = { nullptr };
	class CUI_Slot*			m_pUISlot = { nullptr };

public:
	static CInventory* Create();
	virtual void Free() override;
};

END
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
	_uint*	Get_NumCubic() { return	&m_iNumCubic; }
	_uint	Get_Weapon(_uint iKey);
	_bool	Get_HaveItem(CItem::ITEM eItem);
	_bool	Get_isPotionMax();

	void	Add_Item(class CItem* pItem);
	void	Add_Cubic(_uint iNumCubic) { m_iNumCubic += iNumCubic; }
	void	Add_Potion();

	void	Use_Potion();
	void	Select_Item();

	void	Set_Using(_bool isUsing) { m_isUsing = isUsing; }


public:
	HRESULT Initialize();
	void	Tick(_float fTimeDelta);

private:
	_bool	m_isUsing = { false };

	_uint	m_iMaxItem = { 4 };

	_uint	m_iSelectRow = { 1 };
	_uint	m_iSelectColumn = { 0 };

private:
	vector<class CItem*>	m_Items[CItem::TYPE_END];
	vector<_uint>			m_iNumItems[CItem::TYPE_END];

private:
	_uint					m_iNumCubic = { 10000 };
	_uint					m_iNumBreak = { 0 };
	_uint					m_iNumPotion = { 0 };

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	class CUI_Inventory*	m_pUIInventory = { nullptr };
	vector<class CUI_Item*> m_pUIItems[CItem::TYPE_END];
	class CUI_Slot*			m_pUISlot = { nullptr };
	class CUI_Potion*		m_pUIPotion = { nullptr };
	class CUI_Obtain*		m_pUIObtain = { nullptr };
	vector<class CUI_Font*> m_pUIFonts;
	class CUI_Font*			m_pUIFont_Gem;
	class CUI_Font*			m_pUIFont_Break;

private:
	_uint					Set_WeaponIndex(CItem::ITEM eItem);

public:
	static CInventory* Create();
	virtual void Free() override;
};

END
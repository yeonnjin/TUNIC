#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CItem final : public CBase
{
public:
	enum ITEM_TYPE { TYPE_UTILE, TYPE_USE, TYPE_WEAPON, TYPE_END };

	enum ITEM {
		ITEM_CUBIC, ITEM_BREAK, ITEM_SHIELD, ITEM_DASH, ITEM_HP,
		ITEM_MP, ITEM_BOMB, ITEM_ICE, ITEM_STICK, ITEM_SWORD,
		ITEM_WAND, ITEM_END
	};

public:
	CItem();
	virtual ~CItem() = default;

public:
	ITEM_TYPE	Get_ItemType() { return m_eType; }
	ITEM		Get_Item() { return m_eItem; }

public:
	HRESULT Initialize();
	void	Tick(_float fTimeDelta);

private:
	ITEM_TYPE	m_eType = { TYPE_END };
	ITEM		m_eItem = { ITEM_END };

	_float2		m_vTexturePos = {};

private:
	//class CGameInstance* m_pGameInstance = { nullptr };

private:
	void		Set_TextureInfo();

public:
	static CItem* Create();
	virtual void Free() override;
};

END
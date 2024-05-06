#pragma once

#include "Client_Defines.h"
#include "InteractiveObject.h"

BEGIN(Client)

class CItem final : public CInteractiveObject
{
public:
	enum ITEM_TYPE { TYPE_UTILE, TYPE_USE, TYPE_WEAPON, TYPE_END };

	enum ITEM {
		ITEM_CUBIC, ITEM_BREAK, ITEM_SHIELD, ITEM_DASH, ITEM_HP,
		ITEM_MP, ITEM_BOMB, ITEM_ICE, ITEM_STICK, ITEM_SWORD,
		ITEM_WAND, ITEM_POTION, ITEM_POTION_NULL, ITEM_END
	};

	// TODO: Potion_Null -> 슬롯 쪽으로 옮기고 삭제하기

public:
	typedef struct Item_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		//_float4x4		TransformMatrix;
		_vector			vPosition;
		wstring			strModelComTag;
		_bool			isUseMaterial;
		_bool			isPotion = { false };
		_vector			vMtrlDiffuse = {};
		ITEM			eItem;
	}ITEM_DESC;

public:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CItem& rhs);
	virtual ~CItem() = default;

public:
	ITEM_TYPE	Get_ItemType() { return m_eItemType; }
	ITEM		Get_Item() { return m_eItem; }
	_bool		Get_isOK();
	_uint*		Get_Count_Ptr() { return &m_iCount; }

	void		Set_ItemType(ITEM_TYPE eType) { m_eItemType = eType; }
	void		Set_Item(ITEM eItem) { m_eItem = eItem; }

	void		Plus_Count(_bool isPlus) { m_iCount += (true == isPlus) ? 1 : -1; };
	HRESULT		Use_Item();

	// SHOP
	void		Set_ShopItem(class CNPC_Merchant* pNPC);
	void		Select_Item();
	CItem*		Buy_Item(_uint* iNumCubic);
	void		Exit_Shop();

public:
	virtual		HRESULT		Initialize_Prototype() override;
	virtual		HRESULT		Initialize(void* pArg) override;
	virtual		HRESULT		Tick(_float fTimeDelta)override;
	virtual		void		Late_Tick(_float fTimeDelta) override;
	virtual		HRESULT		Render() override;

private:
	ITEM_TYPE				m_eItemType = { TYPE_END };
	ITEM					m_eItem = { ITEM_END };

	_bool					m_isUseMaterial = { false };
	_bool					m_isPotion = { false };
	_bool					m_isShop = { false };

	_uint					m_iTextureIndex = { 0 };
	_float2					m_vTexturePos = {};

	_vector					m_vPosition = {};
	_vector					m_vMtrlDiffuse = {};

	_uint					m_iCount = { 1 };
	_uint					m_iPrice = { 1000 };

	_uint					m_iPlusHP = { 1 };
	_float					m_fPlusMP = { 1.f };

private:
	wstring					m_strModelComTag = {};
	CModel*					m_pModelItemCom = { nullptr };

	class CNPC_Merchant*	m_pNPC = { nullptr };

private:
	void					Set_Information();

	virtual		HRESULT		Add_Components();
	virtual		HRESULT		Bind_ShaderResources();


public:
	static CItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CMonster을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
	void Damage_Event() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CTransform;
END

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

	void		Set_ItemType(ITEM_TYPE eType) { m_eType = eType; }
	void		Set_Item(ITEM eItem) { m_eItem = eItem; }

	void		Plus_Count(_bool isPlus) { m_iCount += (true == isPlus) ? 1 : -1; };

public:
	HRESULT		Initialize();
	void		Tick(_float fTimeDelta);
	void		Late_Tick(_float fTimeDelta);
	HRESULT		Render();

private:
	ITEM_TYPE	m_eType = { TYPE_END };
	ITEM		m_eItem = { ITEM_END };

	_uint		m_iTextureIndex = { 0 };
	_float2		m_vTexturePos = {};

	_uint		m_iCount = { 1 };

private:
	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CCollider*	m_pColliderCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };

	class CGameInstance* m_pGameInstance = { nullptr };

private:
	void		Set_TextureInfo();
	HRESULT		Add_Components();
	HRESULT		Bind_ShaderResources();

public:
	static CItem* Create();
	virtual void Free() override;
};

END
#pragma once

#include "Sprite.h"

BEGIN(Client)

class CSprite_Attack abstract : public CSprite
{
protected:
	CSprite_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSprite_Attack() = default;

public:
	void	Set_Using(_bool isUsing, CTransform* pTargetTransform);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_bool			m_isUsing = { false };

	_uint			m_iCurIndex = { 0 };
	_uint			m_iMaxIndex = { 4 };

	_float			m_fSize = { 64.f };

	_float			m_fAccTickTime = { 0.f };
	_float			m_fTickTime = { 0.1f };

protected:
	CTransform* m_pTargetTransform = { nullptr };

protected:
	virtual HRESULT	Add_Components();
	virtual HRESULT Bind_ShaderResources();

public:
	virtual void Free() override;
};

END


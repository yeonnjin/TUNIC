#pragma once

#include "Client_Defines.h"
#include "InteractiveObject.h"

BEGIN(Client)

class CObject_Ladder final : public CInteractiveObject
{
public:
	typedef struct Ladder_Desc : public CInteractiveObject::GAMEOBJECT_DESC
	{
		_bool			isUpper;
		_bool			isRotation;
		_bool			isEnd;
		_vector			vPosition;
		_uint			iIndex;
	}LADDER_DESC;

public:
	CObject_Ladder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CObject_Ladder(const CObject_Ladder& rhs);
	virtual ~CObject_Ladder() = default;

public:
	_bool		Get_isUpper() { return m_isUpper; }
	_bool		Get_isEnd() { return m_isEnd; }
	_uint		Get_Index() { return m_iIndex; }

public:
	virtual		HRESULT		Initialize_Prototype() override;
	virtual		HRESULT		Initialize(void* pArg) override;
	virtual		HRESULT		Tick(_float fTimeDelta)override;
	virtual		void		Late_Tick(_float fTimeDelta) override;
	virtual		HRESULT		Render() override;

private:
	_bool		m_isUpper = { true };
	_bool		m_isRotation = { false };
	_bool		m_isEnd = { false };

	_uint		m_iIndex = { 0 };

private:
	virtual		HRESULT		Add_Components();
	virtual		HRESULT		Bind_ShaderResources();
	virtual		void		Compute_ColliderMatrix();

public:
	static CObject_Ladder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CMonster을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END
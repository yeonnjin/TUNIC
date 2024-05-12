#pragma once

#include "Client_Defines.h"
#include "InteractiveObject.h"

BEGIN(Client)

class CTrigger_Scene final : public CInteractiveObject
{
public:
	/*typedef struct Trigger_Scene_Desc : public CInteractiveObject::GAMEOBJECT_DESC
	{
		_vector		vPosition;
		_float3		vSize = { 1.f, 1.f, 1.f };
	}TRIGGER_SCENE_DESC;*/

private:
	CTrigger_Scene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrigger_Scene(const CTrigger_Scene& rhs);
	virtual ~CTrigger_Scene() = default;

public:

public:
	virtual		HRESULT		Initialize_Prototype() override;
	virtual		HRESULT		Initialize(void* pArg) override;
	virtual		HRESULT		Tick(_float fTimeDelta)override;
	virtual		void		Late_Tick(_float fTimeDelta) override;
	virtual		HRESULT		Render() override;

private:
	_bool		m_isTrigger = { false };

private:
	virtual		HRESULT		Add_Components();
	virtual		HRESULT		Bind_ShaderResources();
	virtual		void		Compute_ColliderMatrix();

public:
	static CTrigger_Scene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CMonster을(를) 통해 상속됨
	void Collision_Event(Engine::CGameObject* pGameObject) override;
};

END




#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CAnimator;
class CShader;
class CCollider;
END

BEGIN(Client)

class CMonster abstract : public CGameObject
{
public: 
	typedef struct Monster_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4x4		TransformMatrix;
		wstring			strModelComTag;
	}MONSTER_DESC;

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:	
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	wstring			m_strModelComTag = {};
	CAnimator*		m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };

protected:
	virtual HRESULT	Add_Components();
	virtual HRESULT	Bind_ShaderResources();

	virtual HRESULT	Add_States() = 0;
	virtual void	Update_State() = 0;
	virtual void	Set_Animation() = 0;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
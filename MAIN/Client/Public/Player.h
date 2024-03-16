#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CAnimator;
class CPartObject;
END

/* 귀여운 여우 친구 */

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum STATE {
		STATE_IDLE = 0x01,
		STATE_RUN = 0x02,
		STATE_ATTCK = 0x04,
	};

public: // GAMEOBJECT_DESC 로 올리면 카메라에서 터짐;;
	typedef struct Player_Desc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4x4		TransformMatrix;
		wstring			strModelComTag;
	}PLAYER_DESC;

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	map<const wstring, CPartObject*>	m_PartObjects;
	_ubyte								m_eState = {};

private:
	wstring			m_strModelComTag = {};
	CAnimator*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };

private:
	HRESULT			Add_Components();
	HRESULT			Add_PartObjects();
	HRESULT			Bind_ShaderResources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CEasing;
END

BEGIN(Client)

class CCamera_Focus final : public CCamera
{
public:
	enum STATE { START, UPDATE, _END, NONE, STATE_END };

public:
	typedef struct Camera_Focus_Desc : public CCamera::CAMERA_DESC
	{
		class CTransform* pTargetTransform;
	}CAMERA_FOCUS_DESC;

private:
	CCamera_Focus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Focus(const CCamera_Focus& rhs);
	virtual ~CCamera_Focus() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_vector				m_CamDistance = {};
	_float				m_fCurTime = {};
	_bool				m_isFirst = { true };

private:
	class CEasing*		m_pEasing;
	class CTransform*	m_pTargetTransform;
	class CPlayer*		m_pPlayer;

public:
	static CCamera_Focus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
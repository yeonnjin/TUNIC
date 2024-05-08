#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CTransform;
class CEasing;
END

BEGIN(Client)

class CCamera_LockOn final : public CCamera
{
public:
	typedef struct Camera_LockOn_Desc : public CCamera::CAMERA_DESC
	{
		class CTransform* vTargetTransform;
	}CAMERA_LOCKON_DESC;

private:
	CCamera_LockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_LockOn(const CCamera_LockOn& rhs);
	virtual ~CCamera_LockOn() = default;

public:
	void				Set_EnterBoss();

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual HRESULT		Tick(_float fTimeDelta) override;
	virtual void		Late_Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual void		Set_Level(_uint iLevel);

	virtual void		OnEnter(void* pArg);
	virtual void		OnExit();

private:
	_uint				m_iLevel = { LEVEL_END };

	_vector				m_vDistance = {};
	_vector				m_vMidPosition = {};
	_float				m_fHeight = {};
	_vector				m_vTargetPosition = {};
	_vector				m_vLookAt = {};

	_bool				m_isFind = { false };
	_bool				m_isHigh = { false };
	//_bool				m_isExit = { false };  ºÎ¸ð

	_float				m_fMidTime = { 0.f };
	_float				m_fHighTime = { 0.f };
	_float				m_fMoveTime = { 0.f };
	_float				m_fIdleTime = { 0.f };
	_float				m_fOriginTime = { 0.f };

	_float				m_fRadian = { XMConvertToRadians(60.f) };

private:
	CTransform*			m_pPlayerTransform = { nullptr };
	CTransform*			m_pTargetTransform = { nullptr };

	class CEasing*		m_pEasing;
	class CPlayer*		m_pPlayer;

private:
	void				Set_Distance();

public:
	static CCamera_LockOn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
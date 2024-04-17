#pragma once

#include "Base.h"

BEGIN(Engine)

class CCamera_Manager : public CBase
{
private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT				Add_Camera(const wstring& strCameraTag, class CCamera* pCamera);
	HRESULT				Change_Camera(const wstring& strCameraTag, void* pArg = nullptr);
	
	_float				Get_Camera_Far();
	HRESULT				Set_Exit(const wstring& strCameraTag, _bool isExit);
	void				Set_Camera_Level(_uint iLevel);

public:
	HRESULT				Initialize();
	void				Tick(_float fTimeDelta);
	void				Late_Tick(_float fTimeDelta);

private:
	map<const wstring, class CCamera*>	m_Cameras;
	class CCamera*						m_pCurCamera = { nullptr };

private:
	CCamera*			Find_Camera(const wstring& strCameraTag);

public:
	static CCamera_Manager* Create();
	virtual void Free() override;
};

END
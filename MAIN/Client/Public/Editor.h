#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CEditor final : public CGameObject
{
private:
	CEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEditor(const CEditor& rhs);
	virtual ~CEditor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	// TAB
	void			Frame_Tab();

	// PICKING
	
	void			Gizmo(CTransform* pTransform);
	void			Tool_Picking();

	// PLAYER
	void			Tool_PlayerInfo();
	void			Tool_CameraInfo();





	string					WStringToString(wstring& wstr);

private: // GIZMO
	_bool					m_isUsingPicking = { true };
	_bool					m_isUsingGizmo = { false };
	_int					m_iTargetIndex = { -1 };
	CTransform*				m_pGizmoTransform = { nullptr };
	class CPlayer*			m_pPlayer = { nullptr };
	class CMonster_Spinner* m_pTargetObject = { nullptr };

private: // MODEL_LIST
	vector<wstring>			m_strModels;

private: // FILE
	_uint					m_iObjectCount = { 0 };

private: // TEST
	bool show_demo_window = true;
	bool show_another_window = true;
	_float4 clear_color = _float4();

private: // FPS
	_uint m_iFPS = { 0 };
	_uint m_iCount = { 0 };
	_float m_fTime = { 0.f };

public:
	static CEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
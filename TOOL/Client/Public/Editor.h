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
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	// TAB
	void			Frame_Tab();

	// PICKING
	HRESULT			Test_Picking();
	HRESULT			Test_Mesh_Picking();
	HRESULT			Map_Picking();
	void			Test();
	void			Gizmo(CTransform* pTransform);
	void			Tool_Picking();

	// MODEL_LIST
	HRESULT			Load_Model();
	void			Tool_Model_List();

	// MAP
	HRESULT			Save_Object_File(const wstring& strLayerTag);
	HRESULT			Load_Object_File(const wstring& strLayerTag);
	void			Tool_Map_File();

	// FILE
	HRESULT			Save_Model_File(const wstring& strLayerTag);
	HRESULT			Load_Model_File(const wstring& strLayerTag);
	void			Tool_Model_File();





	string					WStringToString(wstring& wstr);

private: // GIZMO
	_bool					m_isUsingPicking = { true };
	_bool					m_isUsingGizmo = { false };
	_int					m_iTargetIndex = { -1 };
	CTransform*				m_pGizmoTransform = { nullptr };

private: // MODEL_LIST
	vector<wstring>			m_strModels;

private: // FILE
	_uint					m_iObjectCount = { 0 };

private: // TEST
	bool show_demo_window = true;
	bool show_another_window = true;
	_float4 clear_color = _float4();

public:
	static CEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
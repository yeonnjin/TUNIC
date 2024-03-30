#pragma once

#include "Base.h"

#include "imgui.h"
#include "Model.h"

BEGIN(Engine)

class CImGui_Manager final: public CBase
{
private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	New_Frame();
	HRESULT Render();

	// GIZMO
	void	EditTransform(class CTransform* pTransformCom);

	// FILEDIALOG
	void	Open_FileDialog();

	// PROTOTYPE
	HRESULT					Create_Prototype_Model(CModel::TYPE eType, const wstring& strFolderPath);
	_uint					Get_Model_List_Size(CModel::TYPE eType) { return m_strModels[eType].size(); }
	vector<wstring>*		Get_Model_List(CModel::TYPE eType) { return &m_strModels[eType]; }

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	vector<wstring>			m_strModels[CModel::TYPE_END];

public:
	static CImGui_Manager* Create(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
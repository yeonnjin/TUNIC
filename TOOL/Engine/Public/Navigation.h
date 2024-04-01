#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct Navigation_Desc
	{
		_int	iCurrentIndex = { 0 };
	}NAVIGATION_DESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	HRESULT			Add_Cell(class CCell* pCell);
	void			Delete_Cell();
	void			Clear_Cell();

	_int*			Get_Neighbor_Index(_uint iCellIndex);
	void			Set_Neighbor_Index(_uint iCellIndex, _int* pNeighborindex);

	HRESULT			SetUp_Neighbors();

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Prototype(const wstring& strDataFile);
	virtual HRESULT Initialize(void* pArg) override;
	void			Tick(_fmatrix WorldMatrix);
	_bool			isMove(_fvector vPosition);
	//_vector			Go_Sliding(_fvector vPosition, _fvector vLook);
	_vector			Get_Sliding(_fvector vPosition, _fvector vOriginPosition, _fvector vTargetLook, _float fSpeed, _float fTimeDelta);

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif

private:
	vector<class CCell*>	m_Cells;
	static _float4x4		m_WorldMatrix;
	_int					m_iCurrentIndex = { -1 }; // 이 네비게이션을 이용하는 객체가 어떤 셀에 존재하는가?

#ifdef _DEBUG
private:
	class CShader*			m_pShader = { nullptr };
#endif

private:
	//_vector			Calculate_Sliding(_fvector vLook);

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
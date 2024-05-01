#pragma once

#include "UI.h"

BEGIN(Engine)
class CEasing;
END

BEGIN(Client)

class CUI_Arrow final : public CUI
{
public:
	enum ARROW { ARROW_LEFT, ARROW_RIGHT, ARROW_UP, ARROW_DOWN, ARROW_END };
	enum ANSWER { ANSWER_FAIL, ANSWER_OK, ANSWER_SUCCESS, ANSWER_END };

private:
	CUI_Arrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CUI_Arrow() = default;

public:
	ANSWER			Get_Answer() { return m_eAnswer; }

	void			Input_Arrow(ARROW eArrow);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool			m_isUsing = { true };

	_float			m_fSize = { 76.f };

	_float			m_fAlpha = { 1.f };
	_float3			m_vColor = { 1.f, 1.f, 1.f };

	_float			m_fAccShaderTime = { 0.f };
	_float			m_fShaderTime = { 1.f };
	//_float			m_fTotalShaderTime = { 3.f };

	_float2			m_vArrowPosition[16];

	vector<ARROW>	m_vInput;
	vector<ARROW>	m_vAnswer;

	ANSWER			m_eAnswer = { ANSWER_END };

	class CEasing*	m_pEasing = { nullptr };

private:
	virtual HRESULT	Add_Components();
	virtual HRESULT Bind_ShaderResources();
	virtual HRESULT	Set_UIInfo();

	void			Set_Reset();
	void			Set_Answer();
	void			Set_Fail();
	void			Set_Success();

	_bool			Compute_Alpha(_float fTimeDelta);

public:
	static CUI_Arrow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END


#include "stdafx.h"
#include "UI_Arrow.h"

#include "Easing.h"

CUI_Arrow::CUI_Arrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

void CUI_Arrow::Input_Arrow(ARROW eArrow)
{
	m_vInput.push_back(eArrow);

	switch (eArrow)
	{
	case Client::CUI_Arrow::ARROW_LEFT:
		m_UIDescs[m_vInput.size() - 1].iBindTextureIndex = 2;
		break;
	case Client::CUI_Arrow::ARROW_RIGHT:
		m_UIDescs[m_vInput.size() - 1].iBindTextureIndex = 3;
		break;
	case Client::CUI_Arrow::ARROW_UP:
		m_UIDescs[m_vInput.size() - 1].iBindTextureIndex = 0;
		break;
	case Client::CUI_Arrow::ARROW_DOWN:
		m_UIDescs[m_vInput.size() - 1].iBindTextureIndex = 1;
		break;
	case Client::CUI_Arrow::ARROW_END:
		break;
	default:
		break;
	}

	m_UIDescs[m_vInput.size() - 1].isRender = true;

	for (size_t i = 0; i < m_vInput.size(); i++)
	{
		// Æ²·ÈÀ» ¶§ : »¡°£»öÀ¸·Î ±ôºýÀÓ
		if (m_vInput[i] != m_vAnswer[i])
		{
			Set_Fail();
			return;
		}	
	}

	// ¼º°øÇßÀ» ¶§ : ÆÄ¶õ»öÀ¸·Î ±ôºýÀÓ
	if (16 == m_vInput.size())
	{
		Set_Success();
		return;
	}

	// ¸ÂÃèÀ» ¶§
	m_eAnswer = ANSWER_OK;

	return;
}

HRESULT CUI_Arrow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Arrow::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Set_UIInfo()))
		return E_FAIL;

	Set_Answer();

	m_pEasing = CEasing::Get_Instance();
	m_isClone = true;
	Safe_AddRef(m_pEasing);

//#ifndef _DEBUG
//	Safe_AddRef(m_pEasing);
//#endif // _DEBUG  

	return S_OK;
}

HRESULT CUI_Arrow::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	if (true == m_isUsing)
	{
		if (ANSWER_FAIL == m_eAnswer || ANSWER_SUCCESS == m_eAnswer)
		{
			if (true == Compute_Alpha(fTimeDelta))
			{
				if(ANSWER_FAIL == m_eAnswer)
				{
					Set_Reset();
				}
				else
				{
					m_isUsing = false;
				}
				m_eAnswer = ANSWER_END;
			}
		}
	}

	return S_OK;
}

void CUI_Arrow::Late_Tick(_float fTimeDelta)
{
	if (true == m_isUsing)
		__super::Late_Tick(fTimeDelta);
}

HRESULT CUI_Arrow::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Arrow::Add_Components()
{
	// SHADER, VIBUFFER
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_PUZZLE, TEXT("Prototype_Component_Texture_UI_Arrow"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Arrow::Bind_ShaderResources()
{
	// MATRIX
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	// World
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pUITransformComs[m_iBindTransformIndex]->Get_WorldFloat4x4_Ptr())))
		return E_FAIL;

	// TEXTURE
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iBindTextureIndex)))
		return E_FAIL;

	// COLOR
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBlendColor", &m_vColor, sizeof(_float3))))
		return E_FAIL;

	// ALPHA
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Arrow::Set_UIInfo()
{
	for (size_t i = 0; i < 16; i++)
	{
		m_vArrowPosition[i].x = g_iWinSizeX * -0.5 + 2.f + m_fSize * 0.5f + i * (m_fSize + 4.f);
		m_vArrowPosition[i].y = 0.f;

		CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
		if (nullptr == pTransform)
			return E_FAIL;

		pTransform->Set_Scaled(m_fSize, m_fSize, 1.f);
		pTransform->Set_State(CTransform::STATE_POSITION, _vector{ m_vArrowPosition[i].x, m_vArrowPosition[i].y, 0.9f, 1.f });
		m_pUITransformComs.emplace_back(pTransform);

		UI_DESC tDesc = {};
		tDesc.iBindTextureIndex = 0;
		tDesc.iBindTransformIndex = i;
		tDesc.eShader = UI_STAT;
		tDesc.isRender = false;
		m_UIDescs.emplace_back(tDesc);
	}
}

void CUI_Arrow::Set_Reset()
{
	for (auto& tUI : m_UIDescs)
	{
		tUI.isRender = false;
		tUI.eShader = UI_STAT;
	}

	m_vInput.clear();
	m_vInput.shrink_to_fit(); 

	m_fAlpha = 1.f;
	m_vColor = { 1.f, 1.f, 1.f };
	m_eAnswer = ANSWER_OK;
}

void CUI_Arrow::Set_Answer()
{
	m_vAnswer.resize(16);

	m_vAnswer[0] = ARROW_LEFT;
	m_vAnswer[1] = ARROW_UP;

	m_vAnswer[2] = ARROW_RIGHT;
	m_vAnswer[3] = ARROW_UP;

	m_vAnswer[4] = ARROW_RIGHT;
	m_vAnswer[5] = ARROW_DOWN;

	m_vAnswer[6] = ARROW_RIGHT;
	m_vAnswer[7] = ARROW_UP;

	m_vAnswer[8] = ARROW_RIGHT;
	m_vAnswer[9] = ARROW_UP;

	m_vAnswer[10] = ARROW_LEFT;
	m_vAnswer[11] = ARROW_LEFT;

	m_vAnswer[12] = ARROW_LEFT;
	m_vAnswer[13] = ARROW_UP;

	m_vAnswer[14] = ARROW_RIGHT;
	m_vAnswer[15] = ARROW_UP;
}

void CUI_Arrow::Set_Fail()
{
	m_eAnswer = ANSWER_FAIL;
	m_vColor = { 1.f, 0.f, 0.f };

	for (size_t i = 0; i < m_vInput.size(); i++)
	{
		m_UIDescs[i].eShader = UI_ARROW;
	}
}

void CUI_Arrow::Set_Success()
{
	m_eAnswer = ANSWER_SUCCESS;
	m_vColor = { 0.f, 0.f, 1.f };

	for (size_t i = 0; i < m_vInput.size(); i++)
	{
		m_UIDescs[i].eShader = UI_ARROW;
	}
}

_bool CUI_Arrow::Compute_Alpha(_float fTimeDelta)
{
	_bool isFinish = false;

	static _uint iBlink = 0;

	m_fAccShaderTime += fTimeDelta;
	_float fRatio = m_fAccShaderTime / m_fShaderTime;
	if (m_fAccShaderTime >= m_fShaderTime)
	{
		m_fAccShaderTime = 0.f;
		fRatio = 1.f;
		
		//isFinish = true;
	}

	if(0 == iBlink % 2)
		m_fAlpha = m_pEasing->Get_Ease(CEasing::Ease_InSine, 0.f, 0.5f, fRatio);
	else
		m_fAlpha = m_pEasing->Get_Ease(CEasing::Ease_InSine, 0.5f, 0.f, fRatio);

	if(1.f == fRatio)
		++iBlink;

	if (4 == iBlink)
	{
		iBlink = 0;
		isFinish = true;
	}

	return isFinish;
}

CUI_Arrow* CUI_Arrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Arrow* pInstance = new CUI_Arrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CUI_Arrow"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Arrow::Clone(void* pArg)
{
	CUI_Arrow* pInstance = new CUI_Arrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CUI_Arrow"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CUI_Arrow::Free()
{
	__super::Free();

	if(true == m_isClone)
		Safe_Release(m_pEasing);
}
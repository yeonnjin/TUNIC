#include "Light.h"

#include "GameInstance.h"

CLight::CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc)
{
    m_LightDesc = LightDesc;

    return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	/* 빛 연산을 위한 정보들을 던져줌 */
	if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
		return E_FAIL;

	// 1번 패스(두번 째) 사용
	pShader->Begin(1);

	pVIBuffer->Render();

	return E_NOTIMPL;
}

CLight* CLight::Create(const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight();

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX(TEXT("Failed To Create : CLight"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
}

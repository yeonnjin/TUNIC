#include "stdafx.h"
#include "Camera_Telescope.h"

CCamera_Telescope::CCamera_Telescope(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Telescope::CCamera_Telescope(const CCamera_Telescope& rhs)
    : CCamera{ rhs }
{
}

HRESULT CCamera_Telescope::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Telescope::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamera_Telescope::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    __super::Bind_PipeLines();

    return S_OK;
}

void CCamera_Telescope::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Telescope::Render()
{
    return S_OK;
}

void CCamera_Telescope::Set_Level(_uint iLevel)
{
}

void CCamera_Telescope::OnEnter(void* pArg)
{
}

void CCamera_Telescope::OnExit()
{
}

CCamera_Telescope* CCamera_Telescope::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Telescope* pInstance = new CCamera_Telescope(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CCamera_Telescope"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Telescope::Clone(void* pArg)
{
	CCamera_Telescope* pInstance = new CCamera_Telescope(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CCamera_Telescope"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Telescope::Free()
{
	__super::Free();
}

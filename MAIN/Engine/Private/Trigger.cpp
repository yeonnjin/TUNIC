#include "Trigger.h"

CTrigger::CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTrigger::CTrigger(const CTrigger& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CTrigger::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTrigger::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTrigger::Tick(_float fTimeDelta)
{
    if (E_FAIL == __super::Tick(fTimeDelta))
        return E_FAIL;

    return S_OK;
}

void CTrigger::Late_Tick(_float fTimeDelta)
{
}

HRESULT CTrigger::Render()
{
    return S_OK;
}

void CTrigger::Free()
{
    __super::Free();
}

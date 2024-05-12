#include "stdafx.h"
#include "Particle_Sphere.h"

CParticle_Sphere::CParticle_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CParticle{ pDevice, pContext }
{
}

CParticle_Sphere::CParticle_Sphere(const CParticle_Sphere& rhs)
    : CParticle{ rhs }
{
}

HRESULT CParticle_Sphere::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParticle_Sphere::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Sphere::Tick(_float fTimeDelta)
{
    if (FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    return S_OK;
}

void CParticle_Sphere::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CParticle_Sphere::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Sphere::Add_Components()
{
    if (FAILED(__super::Add_Components()))
        return E_FAIL;

    /* For.Com_Model */
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Effect_Sphere";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, wstr,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Sphere::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

CParticle_Sphere* CParticle_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParticle_Sphere* pInstance = new CParticle_Sphere(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CParticle_Sphere"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Sphere::Clone(void* pArg)
{
    CParticle_Sphere* pInstance = new CParticle_Sphere(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CParticle_Sphere"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Sphere::Free()
{
    __super::Free();
}
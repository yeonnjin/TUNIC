#include "stdafx.h"
#include "Dot.h"

#include "GameInstance.h"

CDot::CDot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDot::CDot(const CDot& rhs)
	: CGameObject{rhs}
{
}

HRESULT CDot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&((DOT_DESC*)pArg)->vPosition));

	m_WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	return S_OK;
}

void CDot::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
}

void CDot::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CDot::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

HRESULT CDot::Add_Components()
{
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc{};

	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CDot* CDot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDot* pInstance = new CDot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CDot"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDot::Clone(void* pArg)
{
	CGameObject* pInstance = new CDot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CDot"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDot::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
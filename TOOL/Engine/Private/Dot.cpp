#include "stdafx.h"
#include "Dot.h"

#include "GameInstance.h"
#include "DebugDraw.h"

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
	return S_OK;
}

void CDot::Tick(_fmatrix WorldMatrix)
{
	
}

HRESULT CDot::Render()
{
	

	return S_OK;
}

HRESULT CDot::Add_Components()
{
	/* Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

	/* 로컬상의 정보를 셋팅한다. */
	ColliderDesc.vSize = _float3(1.0f, 1.0f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	/*if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;*/
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
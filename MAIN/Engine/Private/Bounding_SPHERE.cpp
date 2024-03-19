#include "Bounding_SPHERE.h"
#include "DebugDraw.h"

CBounding_SPHERE::CBounding_SPHERE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_SPHERE::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
	BOUNDING_SPHERE_DESC* pDesc = (BOUNDING_SPHERE_DESC*)pBoundingDesc;

	m_pBoundingDesc_Origin = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
	m_pBoundingDesc = new BoundingSphere(*m_pBoundingDesc_Origin);

    return S_OK;
}

void CBounding_SPHERE::Tick(_fmatrix WorldMatrix)
{
	m_pBoundingDesc_Origin->Transform(*m_pBoundingDesc, WorldMatrix);
}

_bool CBounding_SPHERE::Check_Collision(CCollider::TYPE eType, CBounding* pBounding)
{
	void* pTargetBoundingDesc = pBounding->Get_BoundingDesc();

	m_isCollision = { false };

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_isCollision = m_pBoundingDesc->Intersects(*((BoundingBox*)pTargetBoundingDesc));
		break;
	case CCollider::TYPE_OBB:
		m_isCollision = m_pBoundingDesc->Intersects(*((BoundingOrientedBox*)pTargetBoundingDesc));
		break;
	case CCollider::TYPE_SPHERE:
		m_isCollision = m_pBoundingDesc->Intersects(*((BoundingSphere*)pTargetBoundingDesc));
		break;
	case CCollider::TYPE_END:
		break;
	default:
		break;
	}

	return m_isCollision;
}

#ifdef _DEBUG

HRESULT CBounding_SPHERE::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingDesc, true == m_isCollision ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

    return S_OK;
}

#endif // _DEBUG

CBounding_SPHERE* CBounding_SPHERE::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc)
{
	CBounding_SPHERE* pInstance = new CBounding_SPHERE(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed To Create : CBounding_SPHERE"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_SPHERE::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundingDesc);
	Safe_Delete(m_pBoundingDesc_Origin);
}

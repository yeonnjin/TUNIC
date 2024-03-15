#include "Bounding_SPHERE.h"
#include "DebugDraw.h"

CBounding_SPHERE::CBounding_SPHERE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBounding{ pDevice, pContext }
{
}

HRESULT CBounding_SPHERE::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
    return S_OK;
}

#ifdef _DEBUG

HRESULT CBounding_SPHERE::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
    DX::Draw(pBatch, *m_pBoundingDesc);

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
}

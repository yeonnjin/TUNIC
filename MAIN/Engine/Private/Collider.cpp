#include "Collider.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CCollider::CCollider(const CCollider& rhs)
    : CComponent{ rhs }
    , m_eType{ rhs.m_eType }
#ifdef _DEBUG
    , m_pInputLayout{rhs.m_pInputLayout }
    , m_pEffect{ rhs.m_pEffect }
    , m_pBatch{ rhs.m_pBatch }
#endif // DEBUG
{
#ifdef _DEBUG
    Safe_AddRef(m_pInputLayout);
#endif // DEBUG
}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
    m_eType = eType;

#ifdef _DEBUG
    m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

    m_pEffect = new BasicEffect(m_pDevice);

    m_pEffect->SetVertexColorEnabled(true);

    const void* pShaderByteCode = { nullptr };
    size_t iShaderByteCodeLength = { 0 };

    m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

    m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout);

#endif // DEBUG
    
    return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CCollider::Render()
{
    return S_OK;
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX(TEXT("Failed To Create : CCollider"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CComponent* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CCollider"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pInputLayout);

	if (false == m_isCloned)
	{
		Safe_Delete(m_pEffect);
		Safe_Delete(m_pBatch);
	}
#endif // DEBUG

	Safe_Release(m_pBounding);
}
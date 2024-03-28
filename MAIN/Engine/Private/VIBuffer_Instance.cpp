#include "VIBuffer_Instance.h"

CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& rhs)
    : CVIBuffer{ rhs }
    , m_pVBInstance{ rhs.m_pVBInstance }
    , m_iInstanceStride{ rhs.m_iInstanceStride }
    , m_iNumInstance{ rhs.m_iNumInstance }
    , m_iIndexCountPerInstance{ rhs.m_iIndexCountPerInstance }
    , m_iInstanceVertices{ rhs.m_iInstanceVertices }
    , m_InstanceBufferDesc{ rhs.m_InstanceBufferDesc }
    , m_InstanceSubResourceData{ rhs.m_InstanceSubResourceData }
{
    Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Instance::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* pArg)
{
    if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceSubResourceData, &m_pVBInstance)))
        return E_FAIL;

    return S_OK;
}

HRESULT CVIBuffer_Instance::Bind_Buffers()
{
    ID3D11Buffer*   pVertexBuffers[] = {
        m_pVB,
        m_pVBInstance,
    };

    _uint           iVertexStrides[] = {
        m_iVertexStride,
        m_iInstanceStride,
    };

    _uint           iOffsets[] = {
        0,
        0,
    };

    /* 정점 버퍼들을 장치에 바인딩 */
    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);

    /* 인덱스 버퍼들을 장치에 바인딩 */
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

    /* 어떤 형태로 정점들을 이어 그릴 것인지 */
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

    return S_OK;
}

HRESULT CVIBuffer_Instance::Render()
{
    if (nullptr == m_pContext)
        return E_FAIL;

    /* 인덱스 버퍼의 값을 이용하여 정점 버퍼의 정점들을 그림 */
    m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

    return S_OK;
}

void CVIBuffer_Instance::Free()
{
    __super::Free();

    Safe_Release(m_pVBInstance);
}

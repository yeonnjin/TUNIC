#include "VIBuffer_Terrain.h"
#include "GameInstance.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
    : CVIBuffer{ rhs }
    , m_iNumVerticesX{ rhs.m_iNumVerticesX }
    , m_iNumVerticesZ{ rhs.m_iNumVerticesZ }
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const wstring& strHeightMapFilePath)
{
    _ulong dwByte = { 0 };

    HANDLE hFile = CreateFile(strHeightMapFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (0 == hFile)
        return E_FAIL;

    BITMAPFILEHEADER fh;
    BITMAPINFOHEADER ih;

    ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
    ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

    m_iNumVerticesX = ih.biWidth;
    m_iNumVerticesZ = ih.biHeight;
    m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
    m_pVerticesPos = new _float3[m_iNumVertices];

    _uint* pPixel = new _uint[m_iNumVertices];
    ZeroMemory(pPixel, sizeof(_uint) * m_iNumVertices);

    ReadFile(hFile, pPixel, sizeof(_uint) * m_iNumVertices, &dwByte, nullptr);

    CloseHandle(hFile);

    m_iVertexStride = sizeof(VTXPOSNORTEX);
    m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
    m_iIndexStride = sizeof(_uint);
    m_iNumVertexBuffers = 1;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

    ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));

    /* 정점 버퍼의 byte 크기 */
    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = m_iVertexStride;

    VTXPOSNORTEX* pVertices = new VTXPOSNORTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOSNORTEX) * m_iNumVertices);

    for (_uint i = 0; i < m_iNumVerticesZ; ++i)
    {
        for (_uint j = 0; j < m_iNumVerticesX; ++j)
        {
            _uint iIndex = i * m_iNumVerticesX + j;

            pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3(j, (pPixel[iIndex] & 0x000000ff) / 20.f, i);
            pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
            pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
        }
    }

    ZeroMemory(&m_InitialData, sizeof(m_InitialData));
    m_InitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER

    ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));

    /* 인덱스 버퍼의 byte 크기 */
    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = 0;

    _uint* pIndices = new _uint[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

    _uint iNumIndices = { 0 };

    for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
    {
        for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
        {
            _uint iIndex = i * m_iNumVerticesX + j;

            _uint iIndices[] = {
                iIndex + m_iNumVerticesX,
                iIndex + m_iNumVerticesX + 1,
                iIndex + 1,
                iIndex
            };

            /* 오른쪽 위 삼각형의 인덱스 */
            pIndices[iNumIndices++] = iIndices[0];
            pIndices[iNumIndices++] = iIndices[1];
            pIndices[iNumIndices++] = iIndices[2];

            pIndices[iNumIndices++] = iIndices[0];
            pIndices[iNumIndices++] = iIndices[2];
            pIndices[iNumIndices++] = iIndices[3];
        }
    }

    ZeroMemory(&m_InitialData, sizeof(m_InitialData));
    m_InitialData.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
    Safe_Delete_Array(pPixel);

#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
    return S_OK;
}

_float3 CVIBuffer_Terrain::Compute_Picking(const CTransform* pTransform) const
{
    _float3     vRayDir, vRayPos = {};

    m_pGameInstance->Transform_Picking_To_LocalSpace(pTransform, &vRayDir, &vRayPos);

    _float3 vOut = { 0.f, 0.f, 0.f };

    for (size_t i = 0; i < m_iNumVerticesZ - 1; ++i)
    {
        for (size_t j = 0; j < m_iNumVerticesX - 1; ++j)
        {
            _uint   iIndex = i * m_iNumVerticesX + j;

            _uint   iIndices[4] = {
                iIndex + m_iNumVerticesX,
                iIndex + m_iNumVerticesX + 1,
                iIndex + 1,
                iIndex
            };

            _float fDist = {};

            //Intersects(_In_ FXMVECTOR Origin, _In_ FXMVECTOR Direction, _In_ FXMVECTOR V0, _In_ GXMVECTOR V1, _In_ HXMVECTOR V2, _Out_ float& Dist) noexcept;
            _fvector vOrigin = XMLoadFloat3(&vRayPos);
            XMStoreFloat3(&vRayDir, XMVector3Normalize(XMLoadFloat3(&vRayDir)));
            _fvector vDirection = XMLoadFloat3(&vRayDir);
                     
            // 오른쪽 위 삼각형 충돌
            if (true == DirectX::TriangleTests::Intersects(vOrigin, vDirection , XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), XMLoadFloat3(&m_pVerticesPos[iIndices[1]]), XMLoadFloat3(&m_pVerticesPos[iIndices[2]]), fDist))
            {
                vOut = _float3(vRayPos.x + vRayDir.x * fDist, vRayPos.y + vRayDir.y * fDist, vRayPos.z + vRayDir.z * fDist);
                goto exit;
            }
            
            // 왼쪽 아래 삼각형 충돌
            if (true == DirectX::TriangleTests::Intersects(vOrigin, vDirection, XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), XMLoadFloat3(&m_pVerticesPos[iIndices[2]]), XMLoadFloat3(&m_pVerticesPos[iIndices[3]]), fDist))
            {
                vOut = _float3(vRayPos.x + vRayDir.x * fDist, vRayPos.y + vRayDir.y * fDist, vRayPos.z + vRayDir.z * fDist);
                goto exit;
            }
        }
    }

exit:
    XMStoreFloat3(&vOut, XMVector3TransformCoord(XMLoadFloat3(&vOut), pTransform->Get_WorldMatrix()));

    return vOut;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strHeightMapFilePath)
{
    CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(strHeightMapFilePath)))
    {
        MSG_BOX(TEXT("Failed To Create : CVIBuffer_Terrain"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
    CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CVIBuffer_Terrain"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Terrain::Free()
{
    __super::Free();

    /*if(!m_isCloned)
        Safe_Delete_Array(m_pVerticesPos);*/
}
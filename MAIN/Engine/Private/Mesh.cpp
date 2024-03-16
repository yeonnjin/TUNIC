#include "Mesh.h"

#include "GameInstance.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{ pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& rhs)
    : CVIBuffer{ rhs }
    , m_pIndices{ rhs.m_pIndices }
{
}

HRESULT CMesh::Initialize_Prototype(_uint iModelType, MESHFILE* pMeshFile, const vector<CBone*>& Bone)
{
    strcpy_s(m_szName, pMeshFile->szName);
    m_iMaterialIndex = pMeshFile->iMaterialIndex;
    m_iNumVertices = pMeshFile->iNumVertices;

    m_pVerticesPos = new _float3[m_iNumVertices];
    
    m_iNumFaces = pMeshFile->iNumFaces;
    m_iNumIndices = m_iNumFaces * 3;

    m_iIndexStride = sizeof(_uint);
    m_iNumVertexBuffers = 1;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

    HRESULT hr = CModel::TYPE_NONANIM == iModelType ? Ready_Vertices_For_NonAnimModel(pMeshFile) : Ready_Vertices_For_AnimModel(pMeshFile, Bone);
    if (FAILED(hr))
        return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER

    ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));

    m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = 0;

   // _uint* pIndices = new _uint[m_iNumIndices];
    m_pIndices = new _uint[m_iNumIndices];
    ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

    for (size_t i = 0; i < m_iNumIndices; ++i)
    {
        m_pIndices[i] = pMeshFile->pIndices[i];
    }

    ZeroMemory(&m_InitialData, sizeof m_InitialData);
    m_InitialData.pSysMem = m_pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

   

#pragma endregion

    return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CMesh::Stock_Matrices(const vector<CBone*>& Bones, _float4x4* pMeshBoneMatrices)
{
    for (_uint i = 0; i < m_iNumBones; ++i)
        XMStoreFloat4x4(&pMeshBoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * XMLoadFloat4x4(Bones[m_Bones[i]]->Get_CombinedTransformationMatrix()));

    return S_OK;
}

_float3 CMesh::Compute_Picking(const CTransform* pTransform) const
{
    _float3     vRayDir, vRayPos = {};

    m_pGameInstance->Transform_Picking_To_LocalSpace(pTransform, &vRayDir, &vRayPos);

    _float3 vOut = { 0.f, 0.f, 0.f };

    for (size_t i = 0; i < m_iNumFaces; ++i)
    {
        _uint   iIndex = i * 3;

        _uint   iIndices[3] = {
            iIndex,
            iIndex + 1,
            iIndex + 2
        };

        _float fDist = {};

        //Intersects(_In_ FXMVECTOR Origin, _In_ FXMVECTOR Direction, _In_ FXMVECTOR V0, _In_ GXMVECTOR V1, _In_ HXMVECTOR V2, _Out_ float& Dist) noexcept;
        _fvector vOrigin = XMLoadFloat3(&vRayPos);
        XMStoreFloat3(&vRayDir, XMVector3Normalize(XMLoadFloat3(&vRayDir)));
        _fvector vDirection = XMLoadFloat3(&vRayDir);

        // »ï°¢Çü Ãæµ¹
        if (true == DirectX::TriangleTests::Intersects(vOrigin, vDirection, XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), XMLoadFloat3(&m_pVerticesPos[iIndices[1]]), XMLoadFloat3(&m_pVerticesPos[iIndices[2]]), fDist))
        {
            vOut = _float3(vRayPos.x + vRayDir.x * fDist, vRayPos.y + vRayDir.y * fDist, vRayPos.z + vRayDir.z * fDist);
            XMStoreFloat3(&vOut, XMVector3TransformCoord(XMLoadFloat3(&vOut), pTransform->Get_WorldMatrix()));
            return vOut;
        }
    }

    return vOut;
}

HRESULT CMesh::Ready_Vertices_For_NonAnimModel(MESHFILE* pMeshFile)
{
    m_iVertexStride = sizeof(VTXMESH);

    ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));

    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = m_iVertexStride;

    VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

    for (size_t i = 0; i < m_iNumVertices; ++i)
    {
        pVertices[i] = pMeshFile->pMeshVertices[i];
        m_pVerticesPos[i] = pVertices[i].vPosition;
    }

    ZeroMemory(&m_InitialData, sizeof(m_InitialData));
    m_InitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_AnimModel(MESHFILE* pMeshFile, const vector<CBone*>& Bones)
{
    m_iVertexStride = sizeof(VTXANIMMESH);

    ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));

    m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
    m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_BufferDesc.CPUAccessFlags = 0;
    m_BufferDesc.MiscFlags = 0;
    m_BufferDesc.StructureByteStride = m_iVertexStride;

    VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

    // Vertex
    for (size_t i = 0; i < m_iNumVertices; ++i)
    {
        pVertices[i] = pMeshFile->pAnimMeshVertices[i];
        m_pVerticesPos[i] = pVertices[i].vPosition;
    }
    
    // Bone Index
    m_iNumBones = pMeshFile->iNumBones;
    for (size_t i = 0; i < m_iNumBones; ++i)
    {
        // Bone Index
        m_Bones.push_back(pMeshFile->Bones[i]);
    }

    // OffsetMatrix
    for (size_t i = 0; i < pMeshFile->iNumOffsetMatrices; ++i)
    {
        m_OffsetMatrices.push_back(pMeshFile->OffsetMatrices[i]);
    }

    ZeroMemory(&m_InitialData, sizeof(m_InitialData));
    m_InitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iModelType, MESHFILE* pMeshFile, const vector<class CBone*>& Bones)
{
    CMesh* pInstance = new CMesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(iModelType, pMeshFile, Bones)))
    {
        MSG_BOX(TEXT("Failed To Create : CMesh"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
    return nullptr;
}

void CMesh::Free()
{
    __super::Free();

    Safe_Delete_Array(m_pIndices);
    Safe_Delete_Array(m_pVerticesPos);
}

#include "Mesh.h"

#include "GameInstance.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{ pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& rhs)
    : CVIBuffer{ rhs }
{
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, const vector<CBone*>& Bones, _fmatrix TransformMatrix)
{
    strcpy_s(m_szName, pAIMesh->mName.data);
    m_iMaterialIndex = pAIMesh->mMaterialIndex;
    m_iNumVertices = pAIMesh->mNumVertices;
    m_pVerticesPos = new _float3[m_iNumVertices];   
    //m_iVertexStride = sizeof(VTXMESH);
    m_iNumFaces = pAIMesh->mNumFaces;
    m_iNumIndices = m_iNumFaces * 3;
    //m_pIndices = new _uint[m_iNumIndices];
    m_iIndexStride = sizeof(_uint);
    m_iNumVertexBuffers = 1;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

    HRESULT hr = CModel::TYPE_NONANIM == eModelType ? Ready_Vertices_For_NonAnimModel(pAIMesh, TransformMatrix) : Ready_Vertices_For_AnimModel(pAIMesh, Bones);
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

    _uint* pIndices = new _uint[m_iNumIndices];
    _uint* m_pIndices = new _uint[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

    _uint iNumIndices = { 0 };

    for (size_t i = 0; i < pAIMesh->mNumFaces; ++i)
    {
        pIndices[iNumIndices++] = m_pIndices[iNumIndices] = pAIMesh->mFaces[i].mIndices[0];
        pIndices[iNumIndices++] = m_pIndices[iNumIndices] = pAIMesh->mFaces[i].mIndices[1];
        pIndices[iNumIndices++] = m_pIndices[iNumIndices] = pAIMesh->mFaces[i].mIndices[2];
    }

    ZeroMemory(&m_InitialData, sizeof m_InitialData);
    m_InitialData.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);

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

        // 삼각형 충돌
        if (true == DirectX::TriangleTests::Intersects(vOrigin, vDirection, XMLoadFloat3(&m_pVerticesPos[iIndices[0]]), XMLoadFloat3(&m_pVerticesPos[iIndices[1]]), XMLoadFloat3(&m_pVerticesPos[iIndices[2]]), fDist))
        {
            vOut = _float3(vRayPos.x + vRayDir.x * fDist, vRayPos.y + vRayDir.y * fDist, vRayPos.z + vRayDir.z * fDist);
            XMStoreFloat3(&vOut, XMVector3TransformCoord(XMLoadFloat3(&vOut), pTransform->Get_WorldMatrix()));
            return vOut;
        }
    }

    return vOut;
}

HRESULT CMesh::Ready_Vertices_For_NonAnimModel(const aiMesh* pAIMesh, _fmatrix TransformationMatrix)
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
        memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
        m_pVerticesPos[i] = pVertices[i].vPosition;
        memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
        memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2)); // 8개 까지 가질 수 있으므로 2차원 배열로 선언, 맵핑 이상하면 숫자 넘겨보면서 확인
        memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
    }

    ZeroMemory(&m_InitialData, sizeof(m_InitialData));
    m_InitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_AnimModel(const aiMesh* pAIMesh, const vector<CBone*>& Bones)
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

    for (size_t i = 0; i < m_iNumVertices; ++i)
    {
        memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
        m_pVerticesPos[i] = pVertices[i].vPosition;
        memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
        memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2)); // 8개 까지 가질 수 있으므로 2차원 배열로 선언, 맵핑 이상하면 숫자 넘겨보면서 확인
        memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
    }

    /* 해당 메쉬와 관련된 Bone들의 정보를 정리, 저장 */

    // 이 메쉬와 관련된 Bone이 전체 Bone에서 몇 번째 인덱스인지 저장 
    // 같은 이름을 찾음 -> 메쉬가 가지고 있는 Bone 리스트의 이름과 Bone 본인의 이름이 같음
    m_iNumBones = pAIMesh->mNumBones;

    for (size_t i = 0; i < m_iNumBones; ++i)
    {
        aiBone* pAIBone = pAIMesh->mBones[i];

        // Offset Matrix
        _float4x4   OffsetMatrix;
        memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
        XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

        m_OffsetMatrices.push_back(OffsetMatrix);

        // Bone Index
        _int    iBoneIndex = { -1 };

        auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
        {
            ++iBoneIndex;
            return pBone->Compare_Name(pAIBone->mName.data);
        });

        m_Bones.push_back(iBoneIndex);

        // 찾은 Bone은 몇 개의 정점들에게 영향을 주는 지 확인, 저장
        _uint iNumWeights = pAIBone->mNumWeights;

        for (size_t j = 0; j < iNumWeights; ++j)
        {
            // 정점 별로 최대 4개의 뼈로부터 가중치를 받을 수 있기 때문에 비워져있는 값만 하나씩 채워줌
            if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
            {
                pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
                pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
            }

            else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
            {
                pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
                pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
            }

            else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
            {
                pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
                pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
            }

            else if (0.f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
            {
                pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
                pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
            }
        }
    }

    // Offset Matrix 예외 처리
    if (0 == m_iNumBones)
    {
        m_iNumBones = 1;

        _int    iBoneIndex = { -1 };

        auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
        {
            ++iBoneIndex;
            return pBone->Compare_Name(m_szName);
        });

        m_Bones.push_back(iBoneIndex);

        _float4x4   OffsetMatrix;

        XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

        m_OffsetMatrices.push_back(OffsetMatrix);
    }



    ZeroMemory(&m_InitialData, sizeof(m_InitialData));
    m_InitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;;
}

HRESULT CMesh::Ready_MeshFile()
{
    strcpy_s(m_tMeshFile.szName, m_szName);

    m_tMeshFile.iMaterialIndex = m_iMaterialIndex;

    m_tMeshFile.iNumFaces = m_iNumFaces;

    m_tMeshFile.iNumBones = m_iNumBones;
    m_tMeshFile.Bones = m_Bones;

    m_tMeshFile.pVerticesPos = m_pVerticesPos;
    m_tMeshFile.pIndices = m_pIndices;

    m_tMeshFile.iNumOffsetMatrices = m_OffsetMatrices.size();
    m_tMeshFile.OffsetMatrices = m_OffsetMatrices;

    return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, const vector<CBone*>& Bones, _fmatrix TransformMatrix)
{
    CMesh* pInstance = new CMesh(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, Bones, TransformMatrix)))
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
}

    

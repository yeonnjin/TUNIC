
#include "stdafx.h"
#include "Map_Object.h"

CMap_Object::CMap_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMap_Object::CMap_Object(const CMap_Object& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CMap_Object::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMap_Object::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (nullptr == pArg)
        return E_FAIL;

    MAPOBJ_DESC* pDesc = (MAPOBJ_DESC*)pArg;

    // 맵 오브젝트 로드 시
    if (true == pDesc->isLoad)
    {
        m_pTransformCom->Set_WorldMatrix(pDesc->TransformMatrix);
    }
    // 피킹 시
    else
    {
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
    }
    
    m_strModelComTag = pDesc->strModelComTag;
    

    if (FAILED(Add_Components()))
        return E_FAIL;


    return S_OK;
}

void CMap_Object::Tick(_float fTimeDelta)
{
}

void CMap_Object::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CMap_Object::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; ++i)
    {

        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", i, /*aiTextureType_HEIGHT*/aiTextureType_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CMap_Object::Add_Components()
{
    ///* For.Com_Shader */
    //if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
    //    TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
    //    return E_FAIL;

    ///* For.Com_Model */
    //if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Map"),
    //    TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
    //    return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_TOOL_MAP, m_strModelComTag,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMap_Object::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMap_Object::Ready_MapObj_File()
{
    // TransformMatrix
    m_tMapObjFile.TransformMatrix = m_pTransformCom->Get_WorldFloat4x4();

    // ModelComTag
    _int size = WideCharToMultiByte(CP_UTF8, 0, m_strModelComTag.c_str(), -1, NULL, 0, NULL, NULL);
    _char* buffer = new _char[MAX_PATH];
    ZeroMemory(buffer, sizeof(_char) * MAX_PATH);
    WideCharToMultiByte(CP_UTF8, 0, m_strModelComTag.c_str(), -1, buffer, MAX_PATH - 1, NULL, NULL);
    memcpy(&m_tMapObjFile.szModelComTag, buffer, sizeof(_char) * MAX_PATH);
    delete[] buffer;

    return S_OK;
}

CMap_Object* CMap_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMap_Object* pInstance = new CMap_Object(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CMap_Object"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMap_Object::Clone(void* pArg)
{
    CMap_Object* pInstance = new CMap_Object(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CMap_Object"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMap_Object::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}

#include "stdafx.h"
#include "Map.h"

CMap::CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMap::CMap(const CMap& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CMap::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMap::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    MAP_DESC* pDesc = (MAP_DESC*)pArg;

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

HRESULT CMap::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    m_pNavigationCom->Tick(m_pTransformCom->Get_WorldMatrix());

    return S_OK;
}

void CMap::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif
}

HRESULT CMap::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();
    for (size_t i = 0; i < iNumMeshes; ++i)
    {

        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

//#ifdef _DEBUG
//    m_pNavigationCom->Render();
//#endif // _DEBUG

    return S_OK;
}

HRESULT CMap::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMeshMap"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_strModelComTag,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* For.Com_Navigation */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom)))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CMap::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float fCamFar = m_pGameInstance->Get_Camera_Far();
    if(FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
        return E_FAIL;

    /*const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    if (nullptr == pLightDesc)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
        return E_FAIL;*/

    return S_OK;
}

HRESULT CMap::Ready_MapObj_File()
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

CMap* CMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMap* pInstance = new CMap(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CMap"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMap::Clone(void* pArg)
{
    CMap* pInstance = new CMap(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Create : CMap"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMap::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}

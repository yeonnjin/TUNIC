#include "stdafx.h"
#include "Test_Object.h"

CTest_Object::CTest_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CTest_Object::CTest_Object(const CTest_Object& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CTest_Object::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTest_Object::Initialize(void* pArg)
{
    GAMEOBJECT_DESC		GameObjectDesc{};

    GameObjectDesc.fSpeedPerSec = 10.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    _float4 vPosition = { 1.f, 1.f, 1.f, 1.f };
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    
    if (nullptr != pArg)
    {
        TEST_DESC* pDesc = (TEST_DESC*)pArg;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
    }

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CTest_Object::Tick(_float fTimeDelta)
{

}

void CTest_Object::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTest_Object::Render()
{
    if (FAILED(Bind_ShaderMatrix()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; ++i)
    {
        m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE);

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CTest_Object::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Fiona"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;
    /*if (FAILED(__super::Add_Component(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Rock"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;*/

    return S_OK;
}

HRESULT CTest_Object::Bind_ShaderMatrix()
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

CTest_Object* CTest_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTest_Object* pInstance = new CTest_Object(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CTest_Object"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTest_Object::Clone(void* pArg)
{
    CTest_Object* pInstance = new CTest_Object(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Create : CTest_Object"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTest_Object::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}

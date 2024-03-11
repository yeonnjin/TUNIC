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

    if (FAILED(Add_Components()))
        return E_FAIL;

    _float4 vPosition = { 1.f, 2.f, 1.f, 1.f };
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    return S_OK;
}

void CMap::Tick(_float fTimeDelta)
{
}

void CMap::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CMap::Render()
{
    //D3D11_RASTERIZER_DESC rasterDesc;
    //ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
    //rasterDesc.FillMode = D3D11_FILL_SOLID; // 또는 D3D11_FILL_WIREFRAME
    //rasterDesc.CullMode = D3D11_CULL_NONE;
    //rasterDesc.FrontCounterClockwise = false; // 시계 방향을 전면으로 설정
    //rasterDesc.DepthClipEnable = true; // 깊이 클립 활성화

    //ID3D11RasterizerState* pRasterState = nullptr;
    //HRESULT hr = m_pDevice->CreateRasterizerState(&rasterDesc, &pRasterState);
    //if (SUCCEEDED(hr))
    //{
    //    m_pContext->RSSetState(pRasterState);
    //    // 사용 후에는 반드시 Release를 호출해야 합니다.

        if (FAILED(Bind_ShaderResources()))
            return E_FAIL;

        _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

        for (size_t i = 0; i < iNumMeshes; ++i)
        {

            if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
                return E_FAIL;

            /*if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
                return E_FAIL;

            if (FAILED(m_pTextureCom[TYPE_NORMAL]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture", 0)))
                return E_FAIL;

            if (FAILED(m_pTextureCom[TYPE_SPECULAR]->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", 0)))
                return E_FAIL;*/


            /* if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
                 return E_FAIL;*/

            if (FAILED(m_pShaderCom->Begin(0)))
                return E_FAIL;

            m_pModelCom->Render(i);

            /*if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
                return E_FAIL;*/
        }

   /*     
    }

    pRasterState->Release();*/

    return S_OK;
}

HRESULT CMap::Add_Components()
{
    // LEVEL_TOOL_MAP
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Map"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /*if (FAILED(__super::Add_Component(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Fox"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;*/
    /*if (FAILED(__super::Add_Component(LEVEL_TOOL_MAP, TEXT("Prototype_Component_Model_Rock"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;*/

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

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
    //    return E_FAIL;

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

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}

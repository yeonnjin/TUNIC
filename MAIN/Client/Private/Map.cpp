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
    CGameObject::GAMEOBJECT_DESC tDesc{};
    tDesc.fRotationPerSec = XMConvertToRadians(90.f);
    tDesc.fSpeedPerSec = 3.f;

    if (FAILED(__super::Initialize(&tDesc)))
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

    //m_pTransformCom->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f));        

    return S_OK;
}

HRESULT CMap::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (true == m_pGameInstance->Get_DIKeyState(DIK_Z, KEY_PRESS))
    {
        m_pTransformCom->Turn(_vector{ 0.f, 1.f, 0.f, 0.f }, fTimeDelta);
        //Turn_Pivot(_vector{ 0.f, 0.f, -60.f }, _vector{ 0.f, 1.f, 0.f, 0.f }, fTimeDelta);
    }
    if (true == m_pGameInstance->Get_DIKeyState(DIK_X, KEY_PRESS))
    {
        m_pTransformCom->Turn(_vector{ 0.f, 1.f, 0.f, 0.f }, -1.f * fTimeDelta);
        //Turn_Pivot(_vector{ 0.f, 0.f, 60.f }, _vector{ 0.f, 1.f, 0.f, 0.f }, -1.f * fTimeDelta);
    }

    m_pNavigationCom->Tick(m_pTransformCom->Get_WorldMatrix());

    return S_OK;
}

void CMap::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD, this);

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

void CMap::Turn_Pivot(_vector vPivot, _vector vAxis, _float fAngle)
{
    // 회전축의 원점으로 이동
    _matrix TranslationMatrix = XMMatrixTranslation(-vPivot.m128_f32[0], -vPivot.m128_f32[1], -vPivot.m128_f32[2]);

    // Y축 회전
    _matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fAngle);

    // 원래 회전 축으로 이동
    _matrix OriginMatrix = XMMatrixTranslation(vPivot.m128_f32[0], vPivot.m128_f32[1], vPivot.m128_f32[2]);

    // 행렬 조합
    _matrix FinalMatrix = TranslationMatrix * RotationMatrix * OriginMatrix;

    // 최종 변환 행렬을 맵에 적용
    _vector vPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    vPosition = XMVector3TransformCoord(vPosition, FinalMatrix);
    m_pTransformCom->Turn(vAxis, fAngle);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
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

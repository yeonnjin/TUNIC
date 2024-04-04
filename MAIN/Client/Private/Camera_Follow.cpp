#include "stdafx.h"
#include "Camera_Follow.h"

#include "Player.h"
#include "Easing.h"

CCamera_Follow::CCamera_Follow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
    , m_pEasing{ CEasing::Get_Instance() }
{
   //Safe_AddRef(m_pEasing);
}

CCamera_Follow::CCamera_Follow(const CCamera_Follow& rhs)
    : CCamera{ rhs }
    , m_pEasing{ rhs.m_pEasing }
{
    Safe_AddRef(m_pEasing);
}

HRESULT CCamera_Follow::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Follow::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    /*CAMERA_FOLLOW_DESC* pDesc = (CAMERA_FOLLOW_DESC*)pArg;

    m_pTargetTransform = pDesc->pTargetTransform;*/

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTargetTransform = (CTransform*)(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), g_strTransformTag, 0));


    m_vCamDistance = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) - /*m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);*/ _vector{ 0.f, 0.2f, 0.f, 1.f };

    m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0));

    _vector vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vTargetPosition = /*m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);*/ _vector{ 0.f, 0.2f, 0.f, 1.f };

    _vector vTargetLookAt = { vCamPosition.m128_f32[0], vTargetPosition.m128_f32[1], vTargetPosition.m128_f32[2], 1.f };
    m_pTransformCom->Look_At(vTargetLookAt);

    return S_OK;
}

HRESULT CCamera_Follow::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    // TODO: 나중에 지우기
    static _float fRadian = 60.f;
    if (true == m_pGameInstance->Get_DIKeyState(DIK_M, KEY_PRESS))
    {
        /*m_vCamDistance.m128_f32[1] += 1.f;
        m_vCamDistance.m128_f32[2] -= 1.f;*/
        fRadian -= 1.f;
    }

    if (true == m_pGameInstance->Get_DIKeyState(DIK_N, KEY_PRESS))
    {
        /*m_vCamDistance.m128_f32[1] -= 1.f;
        m_vCamDistance.m128_f32[2] += 1.f;*/

        fRadian += 1.f;
    }

    m_fFovy = XMConvertToRadians(fRadian);

    return S_OK;
}

void CCamera_Follow::Late_Tick(_float fTimeDelta)
{
    _vector vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vObjectPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vTargetPosition = vObjectPosition + m_vCamDistance;
    
    if (true == m_pPlayer->isMove())
    {   
        m_fMoveTime += fTimeDelta;
        vCamPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, vCamPosition.m128_f32[0], vTargetPosition.m128_f32[0], min(m_fMoveTime, 0.5f));
        vCamPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, vCamPosition.m128_f32[1], vTargetPosition.m128_f32[1], min(m_fMoveTime, 0.5f));
        vCamPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InQuart, vCamPosition.m128_f32[2], vTargetPosition.m128_f32[2], min(m_fMoveTime, 0.5f));
        vCamPosition.m128_f32[3] = 1.f;       

        m_fIdleTime = 0.f;
    }
    else
    {
        m_fIdleTime += fTimeDelta * 0.01f;
        vCamPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, vCamPosition.m128_f32[0], vTargetPosition.m128_f32[0], min(m_fIdleTime, 1.f));
        vCamPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, vCamPosition.m128_f32[1], vTargetPosition.m128_f32[1], min(m_fIdleTime, 1.f));
        vCamPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, vCamPosition.m128_f32[2], vTargetPosition.m128_f32[2], min(m_fIdleTime, 1.f));
        vCamPosition.m128_f32[3] = 1.f;

        m_fMoveTime = 0.f;
    }

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);

    if (true == m_isLockOn)
    {
        m_fLookTime += fTimeDelta * 0.2f;
        _float fRatio = m_fLookTime / 0.2f;
        if (1 <= fRatio)
        {
            fRatio = 1.f;
            m_fLookTime = 0.f;
            m_isLockOn = false;
        }

        _vector vTargetLookAt;
        vTargetLookAt.m128_f32[0] = vCamPosition.m128_f32[0];
        vTargetLookAt.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, m_vLookAt.m128_f32[1], vObjectPosition.m128_f32[1], fRatio);
        vTargetLookAt.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, m_vLookAt.m128_f32[2], vObjectPosition.m128_f32[2], fRatio);
        vTargetLookAt.m128_f32[3] = 1.f;

        m_pTransformCom->Look_At(vTargetLookAt);
    }
    __super::Bind_PipeLines();
}

HRESULT CCamera_Follow::Render()
{
    return S_OK;
}

void CCamera_Follow::Set_Level(_uint iLevel)
{
    
}

void CCamera_Follow::OnEnter(void* pArg)
{
    m_fMoveTime = 0.f;
    m_fIdleTime = 0.f;
    m_fLookTime = 0.f;
    
    if (nullptr != pArg)
    {
        CAMERA_FOLLOW_DESC* pDesc = (CAMERA_FOLLOW_DESC*)pArg;
        m_vLookAt = pDesc->vLookAt;
        m_isLockOn = pDesc->isLockOn;
    }
    else
    {
        _vector vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
        _vector vTargetPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);
        m_vLookAt = { vCamPosition.m128_f32[0], vTargetPosition.m128_f32[1], vTargetPosition.m128_f32[2], 1.f };
    }
}

void CCamera_Follow::OnExit()
{
}

CCamera_Follow* CCamera_Follow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Follow* pInstance = new CCamera_Follow(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CCamera_Follow"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Follow::Clone(void* pArg)
{
    CCamera_Follow* pInstance = new CCamera_Follow(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CCamera_Follow"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Follow::Free()
{
    __super::Free();

    Safe_Release(m_pEasing);
}
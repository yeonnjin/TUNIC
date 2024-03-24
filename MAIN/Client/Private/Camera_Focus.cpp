#include "stdafx.h"
#include "Camera_Focus.h"

#include "Player.h"

CCamera_Focus::CCamera_Focus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Focus::CCamera_Focus(const CCamera_Focus& rhs)
    : CCamera{ rhs }
{
}

HRESULT CCamera_Focus::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Focus::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    CAMERA_FOCUS_DESC* pDesc = (CAMERA_FOCUS_DESC*)pArg;

    m_pTargetTransform = pDesc->pTargetTransform;

    m_PrePosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);
    m_CurPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_CamDistance = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) - m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);
    
    m_fEaseTime[START] = 0.2f;
    m_fEaseTime[UPDATE] = 1.f;
    m_fEaseTime[_END] = 0.2f;
    m_fEaseTime[NONE] = 0.8f;


    m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0));

    return S_OK;
}

void CCamera_Focus::Tick(_float fTimeDelta)
{
    //// Keyboard
    //if (m_pGameInstance->Get_DIKeyState(DIK_LEFT, KEY_PRESS))
    //{
    //    m_pTransformCom->Go_Left(fTimeDelta);
    //}
    //if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT, KEY_PRESS))
    //{
    //    m_pTransformCom->Go_Right(fTimeDelta);
    //}
    //if (m_pGameInstance->Get_DIKeyState(DIK_UP, KEY_PRESS))
    //{
    //    m_pTransformCom->Go_Straight(fTimeDelta);
    //}
    //if (m_pGameInstance->Get_DIKeyState(DIK_DOWN, KEY_PRESS))
    //{
    //    m_pTransformCom->Go_Backward(fTimeDelta);
    //}

    //// Mouse
    //_long	MouseMove = { 0 };

    //if (true == m_pGameInstance->Get_DIMouseState(DIMKS_WHEEL, KEY_PRESS))
    //{
    //    if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMMS_X))
    //    {
    //        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.1f);
    //    }
    //    if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMMS_Y))
    //    {
    //        m_pTransformCom->Turn(m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.1f);
    //    }
    //}
}

void CCamera_Focus::Late_Tick(_float fTimeDelta)
{
    /*m_PrePosition = m_CurPosition;

    m_CurPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);

    _vector vDiff = m_CurPosition - m_PrePosition;
    _vector vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

    vCamPosition += vDiff;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);  */

    /*
    m_PrePosition = m_CurPosition;
    m_CurPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vMoveDistance = m_CurPosition - m_PrePosition;
    _vector vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

    if (false == XMVector3Equal(_vector{ 0.f, 0.f, 0.f }, vMoveDistance) && (NONE == m_ePreState || STATE_END == m_ePreState))
        m_eState = START;
    else if (true == XMVector3Equal(_vector{ 0.f, 0.f, 0.f }, vMoveDistance) && (START == m_ePreState))
        m_eState = _END;

    m_fCurTime += fTimeDelta;
    _vector vTemp = m_CurPosition + m_CamDistance;
    _float fSpeed = 1.f;
    _float fRatio = XMVector3Length(vMoveDistance).m128_f32[0] ;

    static _bool isFirst = true;

    if (true == isFirst && false == XMVector3Equal(_vector{ 0.f, 0.f, 0.f }, vMoveDistance))
    {
        isFirst = false;
        fSpeed = 0.8f;
        m_fCurTime = 0.f;
    }

    if (m_fCurTime <= 0.2f)
    {
        m_pTransformCom->Move_To_Target(vTemp, fTimeDelta * fSpeed, 0.5f);
    }

    if (m_fCurTime > 0.2f)
    {
        fSpeed = 1.f;
        vCamPosition += vMoveDistance;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);
    }

    if (false == isFirst && true == XMVector3Equal(_vector{ 0.f, 0.f, 0.f }, vMoveDistance))
    {
        fSpeed = 1.f;
        if (true == m_pTransformCom->Move_To_Target(vTemp, fTimeDelta * fSpeed, 0.5f))
        {
            isFirst = true;
            //m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTemp);
        }
    }
    */
    
    //m_PrePosition = m_CurPosition;
    //m_CurPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION);
    //_vector vMoveDistance = m_CurPosition - m_PrePosition;
    //_float4 vCamPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    //_vector vCalculate = m_CurPosition + m_CamDistance;

    //static _float4 vFirstPos;
    //static _bool isFirst = false;
    //if (false == XMVector3Equal(_vector{ 0.f, 0.f, 0.f }, vMoveDistance))
    //{
    //    m_fCurTime += fTimeDelta ;
    //    if (false == isFirst)
    //    {
    //        vFirstPos = vCamPosition;
    //        isFirst = true;
    //    }
    //    vCamPosition.x = EaseInQuart(vCamPosition.x, vCalculate.m128_f32[0], min(m_fCurTime, 0.8f));
    //    vCamPosition.y = EaseInQuart(vCamPosition.y, vCalculate.m128_f32[1], min(m_fCurTime, 0.8f));
    //    vCamPosition.z = EaseInQuart(vCamPosition.z, vCalculate.m128_f32[2], min(m_fCurTime, 0.8f));
    //    vCamPosition.w = 1.f;

    //    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);
    //    m_eState = UPDATE;
    //}
    //else
    //{
    //    m_fCurTime = 0.f;
    //    isFirst = false;
    //}

    //if(true == XMVector3Equal(_vector{ 0.f, 0.f, 0.f }, vMoveDistance) && UPDATE == m_eState)
    //{
    //    m_fCurTime = 0.f;
    //    m_eState = _END;
    //    //vFinalPos = m_CurPosition + m_CamDistance;
    //}
    //
    //if (_END == m_eState && UPDATE != m_eState)
    //{
    //    /*if (true == m_pTransformCom->Move_To_Target(vCalculate, fTimeDelta * 0.2f, 0.2f))
    //        m_eState = NONE;*/
    //    m_fCurTime += fTimeDelta ;
    //    vCamPosition.x = EaseOutQuart(vCamPosition.x, vCalculate.m128_f32[0], min(m_fCurTime, 1.f));
    //    vCamPosition.y = EaseOutQuart(vCamPosition.y, vCalculate.m128_f32[1], min(m_fCurTime, 1.f));
    //    vCamPosition.z = EaseOutQuart(vCamPosition.z, vCalculate.m128_f32[2], min(m_fCurTime, 1.f));
    //    vCamPosition.w = 1.f;

    //    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);
    //}

    /*_vector vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vTargetPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION) + m_CamDistance;

    static _vector vDiff;
    static _bool isFirst = true;

    if (false == XMVector3Equal(vCamPosition, vTargetPosition))
    {
        m_fCurTime += fTimeDelta * 0.5;

        if (m_fCurTime < 1.f)
        {
            vCamPosition.m128_f32[0] = EaseInQuart(vCamPosition.m128_f32[0], vTargetPosition.m128_f32[0], min(m_fCurTime, 1.f));
            vCamPosition.m128_f32[1] = EaseInQuart(vCamPosition.m128_f32[1], vTargetPosition.m128_f32[1], min(m_fCurTime, 1.f));
            vCamPosition.m128_f32[2] = EaseInQuart(vCamPosition.m128_f32[2], vTargetPosition.m128_f32[2], min(m_fCurTime, 1.f));
            vCamPosition.m128_f32[3] = 1.f;
        }
       
        if (m_fCurTime >= 1.f)
        {
            if (true == isFirst)
            {
                vDiff = vTargetPosition - vCamPosition;
                isFirst = false;
            }

            vTargetPosition -= vDiff;

            vCamPosition.m128_f32[0] = EaseInQuart(vCamPosition.m128_f32[0], vTargetPosition.m128_f32[0], min(m_fCurTime, 1.f));
            vCamPosition.m128_f32[1] = EaseInQuart(vCamPosition.m128_f32[1], vTargetPosition.m128_f32[1], min(m_fCurTime, 1.f));
            vCamPosition.m128_f32[2] = EaseInQuart(vCamPosition.m128_f32[2], vTargetPosition.m128_f32[2], min(m_fCurTime, 1.f));
            vCamPosition.m128_f32[3] = 1.f;
        }

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);
    }
    else
    {
        m_fCurTime = 0.f;
        isFirst = true;
    }*/

    _vector vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vTargetPosition = m_pTargetTransform->Get_State_Vector(CTransform::STATE_POSITION) + m_CamDistance;
    
    static _bool isFirst = true;
    
    if (true == m_pPlayer->isMove())
    {   
        if (false == isFirst)
            isFirst = true;

        m_fCurTime += fTimeDelta;
        vCamPosition.m128_f32[0] = EaseInQuart(vCamPosition.m128_f32[0], vTargetPosition.m128_f32[0], min(m_fCurTime, 0.5f));
        vCamPosition.m128_f32[1] = EaseInQuart(vCamPosition.m128_f32[1], vTargetPosition.m128_f32[1], min(m_fCurTime, 0.5f));
        vCamPosition.m128_f32[2] = EaseInQuart(vCamPosition.m128_f32[2], vTargetPosition.m128_f32[2], min(m_fCurTime, 0.5f));
        vCamPosition.m128_f32[3] = 1.f;       
    }
    else if (false == m_pPlayer->isMove())
    {
        if (true == isFirst)
        {
            isFirst = false;
            m_fCurTime = 0.f;
        }

        m_fCurTime += fTimeDelta * 0.03f;
        vCamPosition.m128_f32[0] = EaseOutQuart(vCamPosition.m128_f32[0], vTargetPosition.m128_f32[0], min(m_fCurTime, 1.f));
        vCamPosition.m128_f32[1] = EaseOutQuart(vCamPosition.m128_f32[1], vTargetPosition.m128_f32[1], min(m_fCurTime, 1.f));
        vCamPosition.m128_f32[2] = EaseOutQuart(vCamPosition.m128_f32[2], vTargetPosition.m128_f32[2], min(m_fCurTime, 1.f));
        vCamPosition.m128_f32[3] = 1.f;
    }

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPosition);

    __super::Bind_PipeLines();
}

HRESULT CCamera_Focus::Render()
{
    return S_OK;
}

_float CCamera_Focus::EaseInOutQuad(_float fStart, _float fEnd, _float fValue)
{
    fValue /= .5f;
    fEnd -= fStart;

    if (fValue < 1)
        return fEnd * 0.5f * fValue * fValue + fStart;

    fValue--;

    return -fEnd * 0.5f * (fValue * (fValue - 2) - 1) + fStart;
}

_float CCamera_Focus::EaseInQuart(_float fStart, _float fEnd, _float fValue)
{
    fEnd -= fStart;

    return fEnd * fValue * fValue * fValue * fValue + fStart;
}

_float CCamera_Focus::EaseOutQuart(_float fStart, _float fEnd, _float fValue)
{
    fValue--;
    fEnd -= fStart;
    return -fEnd * (fValue * fValue * fValue * fValue - 1) + fStart;
}

CCamera_Focus* CCamera_Focus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Focus* pInstance = new CCamera_Focus(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CCamera_Focus"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Focus::Clone(void* pArg)
{
    CCamera_Focus* pInstance = new CCamera_Focus(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CCamera_Focus"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Focus::Free()
{
    __super::Free();
}
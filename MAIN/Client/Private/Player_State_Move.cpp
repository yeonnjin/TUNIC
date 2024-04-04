#include "stdafx.h"

#include "Player.h"
#include "Player_State_Move.h"

CPlayer_State_Move::CPlayer_State_Move(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Move::OnStateEnter()
{
    // TODO:연달아서 누르면 블렌딩 안되는 듯

    m_pPlayer->Set_Blending(true, CPlayer::ANIM_WALK_FORWARD);
}

void CPlayer_State_Move::OnStateUpdate(_float fTimeDelta)
{
    CPlayer::LOCKON eLockOn = m_pPlayer->Get_LockOn();

    m_ePreDir = m_eCurDir;
    m_eCurDir = m_pPlayer->Get_Dir();

    // 일반 상태일 때
    if ((CPlayer::LOCK_OFF == eLockOn || CPlayer::LOCK_END == eLockOn))
    {
        switch (m_eCurDir)
        {
        case CPlayer::DIR_FRONT:
            m_vLook = { 0.f, 0.f, 1.f };
            break;
        case CPlayer::DIR_BACK:
            m_vLook = { 0.f, 0.f, -1.f };
            break;
        case CPlayer::DIR_LEFT:
            m_vLook = { 1.f, 0.f, 0.f };
            break;
        case CPlayer::DIR_RIGHT:
            m_vLook = { -1.f, 0.f, 0.f };
            break;
        case CPlayer::DIR_FL:
            m_vLook = { 1.f, 0.f, 1.f };
            break;
        case CPlayer::DIR_FR:
            m_vLook = { -1.f, 0.f, 1.f };
            break;
        case CPlayer::DIR_BL:
            m_vLook = { 1.f, 0.f, -1.f };
            break;
        case CPlayer::DIR_BR:
            m_vLook = { -1.f, 0.f, -1.f };
            break;
        case CPlayer::DIR_END:
            break;
        default:
            break;
        }

        _vector vTargetLook{};
        vTargetLook = XMVector3Normalize(m_vLook);
        _vector PlayerVector = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_LOOK);

        if (false == XMVector3Equal(vTargetLook, XMVector3Normalize(PlayerVector)))
        {
            m_isTurn = true;
        }

        if (true == m_isTurn)
        {
            if (true == ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Turn_Look(&m_vLerpLook, vTargetLook, fTimeDelta))
                m_isTurn = false;
        }

        //((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Look(fTimeDelta, XMLoadFloat3(&m_vLerpLook));

        ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Backward(fTimeDelta);
    }
    // 락온 상태일 때
    else
    {
        // 몬스터가 없을 경우 바라보고 있던 방향을 계속 바라보기 
        // 몬스터가 있을 경우 몬스터 계속 바라보기
        // 시선은 고정한 채 8방향 움직이기 

        if (CPlayer::LOCK_ON_FIND == eLockOn)
        {
            _vector vTargetPosition = m_pPlayer->Get_LockOn_Transform()->Get_State_Vector(CTransform::STATE_POSITION);
            ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Look_At_For_LandOject(vTargetPosition, true);
            //((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Turn_Look(&m_vLerpLook, vTargetPosition, fTimeDelta);
        }

        switch (m_eCurDir)
        {
        case Client::CPlayer::DIR_FRONT:
            ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Front(fTimeDelta);
            break;
        case Client::CPlayer::DIR_FL:
            ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_FL(fTimeDelta);
            break;
        case Client::CPlayer::DIR_LEFT:
            ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_L(fTimeDelta);
            break;
        case Client::CPlayer::DIR_BL:
            ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_BL(fTimeDelta);
            break;
        case Client::CPlayer::DIR_BACK:
            ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Back(fTimeDelta);
            break;
        case Client::CPlayer::DIR_BR:
            ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_BR(fTimeDelta);
            break;
        case Client::CPlayer::DIR_RIGHT:
            ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_R(fTimeDelta);
            break;
        case Client::CPlayer::DIR_FR:
            ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_FR(fTimeDelta);
            break;
        case Client::CPlayer::DIR_END:
            break;
        default:
            break;
        }  
    }  
}

void CPlayer_State_Move::OnStateExit()
{
}

CPlayer_State_Move* CPlayer_State_Move::Create(CPlayer* pPlayer)
{
    CPlayer_State_Move* pInstance = new CPlayer_State_Move(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Move"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Move::Free()
{
    __super::Free();
}
#include "stdafx.h"

#include "Player.h"
#include "Player_State_Dodge.h"

CPlayer_State_Dodge::CPlayer_State_Dodge(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Dodge::OnStateEnter()
{
    CPlayer::LOCKON eLockOn = m_pPlayer->Get_LockOn();

    //CPlayer::DIR eDir = m_pPlayer->Get_Dir();
    _vector vLook = m_pPlayer->Get_Look();

    if ((CPlayer::LOCK_ON_FIND == eLockOn || CPlayer::LOCK_ON_NONE == eLockOn))
    {
        m_vPreRight = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_RIGHT);
        m_vPreUp = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_UP);
        m_vPreLook = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_LOOK);

        dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Look_At_Dir(vLook);
    }

    _float fPlayerSP = m_pPlayer->Get_SP();

    m_eDodge = m_pPlayer->Get_Dodge();

    if(CPlayer::DODGE_ROLL == m_eDodge)
    {
        // 기력이 충분할 때 
        if (fPlayerSP >= m_fSP)
        {
            m_pPlayer->Set_SP_Minus(m_fSP);
            m_pPlayer->Set_Blending(true, CPlayer::ANIM_DODGE);
            m_eAnim = CPlayer::ANIM_DODGE;
        }
        // 기력이 없을 때
        else
        {
            m_pPlayer->Set_SP_Minus(fPlayerSP);
            m_pPlayer->Set_Blending(true, CPlayer::ANIM_DODGE_GARBAGE);
            m_eAnim = CPlayer::ANIM_DODGE_GARBAGE;
        }
    }
    else if (CPlayer::DODGE_DASH == m_eDodge)
    {
        // 기력이 충분할 때 
        if (fPlayerSP >= m_fSP)
        {
            m_pPlayer->Set_SP_Minus(m_fDashSP);
            m_pPlayer->Set_Blending(true, CPlayer::ANIM_HYPERDASH);
            m_eAnim = CPlayer::ANIM_HYPERDASH;
        }
        // 기력이 없을 때
        else
        {
            m_pPlayer->Set_SP_Minus(fPlayerSP);
            m_pPlayer->Set_Blending(true, CPlayer::ANIM_DODGE_GARBAGE);
            m_eAnim = CPlayer::ANIM_DODGE_GARBAGE;
        }
    }
}

void CPlayer_State_Dodge::OnStateUpdate(_float fTimeDelta)
{
    // 일반 닷지 상태일 때
    // 0 ~ 40 : 공격 면역
    if (CPlayer::ANIM_DODGE == m_eAnim)
    {
        _uint iFrame = m_pPlayer->Get_Current_Frame(m_eAnim);
        if (0 <= iFrame && 40 >= iFrame)
            m_pPlayer->Set_isImmune(true);
        else
            m_pPlayer->Set_isImmune(false);
    }

    // 하이퍼 대쉬 상태일 때
    // 0 ~ 20 : 공격 면역
    if (CPlayer::ANIM_HYPERDASH == m_eAnim)
    {
        _uint iFrame = m_pPlayer->Get_Current_Frame(m_eAnim);
        if (0 <= iFrame && 20 >= iFrame)
            m_pPlayer->Set_isImmune(true);
        else
            m_pPlayer->Set_isImmune(false);

        CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
        pPlayerTransform->Go_Straight(fTimeDelta * -5.f);
    }

    if (true == m_pPlayer->Get_isFinished(m_eAnim))
    {
        IF_PLAYER_ISMOVE
            m_pPlayer->Change_State(CPlayer::STATE_MOVE);
        else
            m_pPlayer->Change_State(CPlayer::STATE_IDLE);
    }
}

void CPlayer_State_Dodge::OnStateExit()
{
    CPlayer::LOCKON eLockOn = m_pPlayer->Get_LockOn();

    if ((CPlayer::LOCK_ON_FIND == eLockOn || CPlayer::LOCK_ON_NONE == eLockOn))
    {
        dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Set_State(CTransform::STATE_RIGHT, m_vPreRight);
        dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Set_State(CTransform::STATE_UP, m_vPreUp);
        dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Set_State(CTransform::STATE_LOOK, m_vPreLook);
    }

    m_pPlayer->Set_isImmune(false);
}

CPlayer_State_Dodge* CPlayer_State_Dodge::Create(CPlayer* pPlayer)
{
    CPlayer_State_Dodge* pInstance = new CPlayer_State_Dodge(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Dodge"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Dodge::Free()
{
    __super::Free();
}
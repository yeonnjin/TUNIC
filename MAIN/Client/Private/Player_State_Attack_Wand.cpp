#include "stdafx.h"

#include "Player.h"
#include "Weapon_Wand.h"
#include "Player_State_Attack_Wand.h"

CPlayer_State_Attack_Wand::CPlayer_State_Attack_Wand(CPlayer* pPlayer, CPlayer_Weapon* pWeapon)
{
    m_pPlayer = pPlayer;
    m_pWeapon = pWeapon;
}

void CPlayer_State_Attack_Wand::OnStateEnter()
{
    m_pPlayer->Set_Blending(true, CPlayer::ANIM_USE_WANDBOW);
    m_pPlayer->Set_MP_Minus(m_fMP);
    //m_iKey = m_pWeapon->Get_Key();
}

void CPlayer_State_Attack_Wand::OnStateUpdate(_float fTimeDelta)
{
    // 프레임 6에서 볼트 발사
    if (6 == m_pPlayer->Get_Current_Frame(CPlayer::ANIM_USE_WANDBOW))
    {
        m_pWeapon->Set_isAttackFrame(true);

        CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));

        dynamic_cast<CWeapon_Wand*>(m_pWeapon)->Set_Dir(pPlayerTransform->Get_State_Vector(CTransform::STATE_LOOK));

        m_pGameInstance->Play_Once(TEXT("PLAYER_Wand_Beam.wav"), CSound_Manager::PLAYER);
    }


    if (true == m_pPlayer->Get_isFinished(CPlayer::ANIM_USE_WANDBOW))
    {
        IF_PLAYER_ISMOVE
            m_pPlayer->Change_State(CPlayer::STATE_MOVE);
        else
            m_pPlayer->Change_State(CPlayer::STATE_IDLE);
    }
}

void CPlayer_State_Attack_Wand::OnStateExit()
{
}

CPlayer_State_Attack_Wand* CPlayer_State_Attack_Wand::Create(CPlayer* pPlayer, CPlayer_Weapon* pWeapon)
{
    CPlayer_State_Attack_Wand* pInstance = new CPlayer_State_Attack_Wand(pPlayer, pWeapon);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Attack_Wand"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Attack_Wand::Free()
{
    __super::Free();
}
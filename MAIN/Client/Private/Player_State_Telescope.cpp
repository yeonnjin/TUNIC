#include "stdafx.h"

#include "Player.h"
#include "Player_State_Telescope.h"

CPlayer_State_Telescope::CPlayer_State_Telescope(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Telescope::OnStateEnter()
{
    m_pPlayer->Set_Blending(true, CPlayer::ANIM_IDLE);
}

void CPlayer_State_Telescope::OnStateUpdate(_float fTimeDelta)
{

}

void CPlayer_State_Telescope::OnStateExit()
{
}

CPlayer_State_Telescope* CPlayer_State_Telescope::Create(CPlayer* pPlayer)
{
    CPlayer_State_Telescope* pInstance = new CPlayer_State_Telescope(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Telescope"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Telescope::Free()
{
    __super::Free();
}

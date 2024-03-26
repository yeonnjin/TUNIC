#include "stdafx.h"

#include "Player.h"
#include "Monster_Blob.h"
#include "Blob_State_Idle.h"

CBlob_State_Idle::CBlob_State_Idle(CMonster_Blob* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CBlob_State_Idle::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Blob::ANIM_IDLE);
}

void CBlob_State_Idle::OnStateUpdate(_float fTimeDelta)
{
    _vector vPlayerPosition = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vMonsterPosition = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);

    if (false == m_isDetect && 7 > XMVector3Length(vPlayerPosition - vMonsterPosition).m128_f32[0])
    {
        m_pMonster->Change_State(CMonster_Blob::STATE_JUMP);
    }
}

void CBlob_State_Idle::OnStateExit()
{
    m_isDetect = false;
}

CBlob_State_Idle* CBlob_State_Idle::Create(CMonster_Blob* pMonster, CPlayer* pPlayer)
{
    CBlob_State_Idle* pInstance = new CBlob_State_Idle(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CBlob_State_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBlob_State_Idle::Free()
{
    __super::Free();
}

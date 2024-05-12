#include "stdafx.h"

#include "Player.h"
#include "Player_State_Water.h"

CPlayer_State_Water::CPlayer_State_Water(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Water::OnStateEnter()
{
    // 0. x 가 1.172f m_fMidX 보다 작을 때 : 왼 -> 오 ( 높이 낮아짐 )

    CTransform* pTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
    _float4 vPosition = pTransform->Get_State_Float4(CTransform::STATE_POSITION);
    m_fEnterX = vPosition.x;
    m_fEnterY = vPosition.y;
}

void CPlayer_State_Water::OnStateUpdate(_float fTimeDelta)
{
    IF_PLAYER_ISMOVE
    {
        CPlayer::LOCKON eLockOn = m_pPlayer->Get_LockOn();

        m_ePreDir = m_eCurDir;
        m_eCurDir = m_pPlayer->Get_Dir();
        _vector vLook = m_pPlayer->Get_Look();

        // 일반 상태일 때
        if ((CPlayer::LOCK_OFF == eLockOn || CPlayer::LOCK_END == eLockOn))
        {
            _vector vTargetLook{};
            vTargetLook = XMVector3Normalize(vLook);
            _vector PlayerVector = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_LOOK);

            if (false == XMVector3Equal(vTargetLook, XMVector3Normalize(PlayerVector)))
            {
                m_isTurn = true;
                m_isFirst = true;
            }

            if (true == m_isTurn)
            {
                if (true == ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Turn_Look(vTargetLook, fTimeDelta))
                {
                    m_isTurn = false;
                    m_isFirst = true;
                }
            }

            ((CTransform*)(m_pPlayer->Get_Component(g_strTransformTag)))->Go_Look(fTimeDelta, vTargetLook);
        }
    }

    CTransform* pTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
    _float4 vPosition = pTransform->Get_State_Float4(CTransform::STATE_POSITION);

    if(vPosition.x <= m_fMidX)
    {
        _float fRatio = (vPosition.x) / (m_fMidX);
        _float fHeight = XMVectorLerp(_vector{ m_fEnterY, m_fEnterY, m_fEnterY }, _vector{ m_fMaxDepth, m_fMaxDepth, m_fMaxDepth }, fRatio).m128_f32[0];
        vPosition.y = fHeight;
    }
    else if (vPosition.x > m_fMidX)
    {
        _float fRatio = (m_fMidX) / (m_fMaxX);
        _float fHeight = XMVectorLerp(_vector{ m_fMaxDepth, m_fMaxDepth, m_fMaxDepth }, _vector{ 0.1f, 0.1f, 0.1f }, fRatio).m128_f32[0];
        vPosition.y = fHeight;
    }

    pTransform->Set_State(CTransform::STATE_POSITION, vPosition);

    if (vPosition.x < m_fMidX || vPosition.x > m_fMaxX)
        m_pPlayer->Change_State(CPlayer::STATE_IDLE);
}

void CPlayer_State_Water::OnStateExit()
{
    m_isFirst = { true };
    m_isTurn = { false };
}

CPlayer_State_Water* CPlayer_State_Water::Create(CPlayer* pPlayer)
{
    CPlayer_State_Water* pInstance = new CPlayer_State_Water(pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CPlayer_State_Water"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_State_Water::Free()
{
    __super::Free();
}

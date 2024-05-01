#include "stdafx.h"
#include "Camera_Top.h"

#include "Player.h"
#include "Easing.h"

CCamera_Top::CCamera_Top(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Top::CCamera_Top(const CCamera_Top& rhs)
    : CCamera{ rhs }
	, m_pEasing{ CEasing::Get_Instance() }
{
#ifndef _DEBUG
	Safe_AddRef(m_pEasing);
#endif // _DEBUG   
}

HRESULT CCamera_Top::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Top::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCamera_Top::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
	_vector vPlayerPosition = dynamic_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
	_vector vCamPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	// 카메라 기본 상태
	if (false == m_isDown && false == m_isBack)
	{
		_vector vOrigin = _vector{ 0.f, vPlayerPosition.m128_f32[1], -58.f, 1.f };
		_vector vDir = XMVector3Normalize(vPlayerPosition - vOrigin);

		_vector vTargetPosition = vOrigin + vDir * m_fDistance;
		vTargetPosition.m128_f32[1] = vCamPosition.m128_f32[1];

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTargetPosition);
		vOrigin.m128_f32[1] = vCamPosition.m128_f32[1];
		m_pTransformCom->Look_At(vOrigin);
	}
	// 카메라 내려가기
	else if (true == m_isDown)
	{
		m_fAccDownTime += fTimeDelta;
		_float fRatio = m_fAccDownTime / m_fDownTime;
		if (fRatio >= 1.f)
		{
			fRatio = 1.f;
			m_fAccDownTime = 0.f;
			m_isDown = false;
			m_isBack = true;
		}

		// 위치
		_vector vPosition;
		vPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, vCamPosition.m128_f32[0], m_vDownPosition.m128_f32[0], fRatio);
		vPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, vCamPosition.m128_f32[1], m_vDownPosition.m128_f32[1], fRatio);
		vPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, vCamPosition.m128_f32[2], m_vDownPosition.m128_f32[2], fRatio);
		vPosition.m128_f32[3] = 1.f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		_vector vOrigin = _vector{ 0.f, vPosition.m128_f32[1], -58.f, 1.f };

		// 룩 방향
		_vector vLook;
		vLook.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, vPlayerPosition.m128_f32[0], vOrigin.m128_f32[0], fRatio);
		vLook.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, vPlayerPosition.m128_f32[1], vOrigin.m128_f32[1], fRatio);
		vLook.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, vPlayerPosition.m128_f32[2], vOrigin.m128_f32[2], fRatio);
		vLook.m128_f32[3] = 1.f;

		m_pTransformCom->Look_At(vLook);
	}
	// 카메라 뒤로가기
	else if (true == m_isBack)
	{
		m_fAccBackTime += fTimeDelta;
		_float fRatio = m_fAccBackTime / m_fBackTime;
		if (fRatio >= 1.f)
		{
			fRatio = 1.f;
			m_fAccBackTime = 0.f;
			m_isBack = false;
		}

		_vector vPosition;
		vPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, vCamPosition.m128_f32[0], m_vBackPosition.m128_f32[0], fRatio);
		vPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, vCamPosition.m128_f32[1], m_vBackPosition.m128_f32[1], fRatio);
		vPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_InQuad, vCamPosition.m128_f32[2], m_vBackPosition.m128_f32[2], fRatio);
		vPosition.m128_f32[3] = 1.f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		_vector vOrigin = _vector{ 0.f, vPosition.m128_f32[1], -58.f, 1.f };
		m_pTransformCom->Look_At(vOrigin);

		// 시야각
		_float fRadian = 70.f;
		m_fFovy = m_pEasing->Get_Ease(CEasing::Ease_InQuad, m_fFovy, XMConvertToRadians(fRadian), fRatio);
	}

    __super::Bind_PipeLines();

    return S_OK;
}

void CCamera_Top::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Top::Render()
{
    return S_OK;
}

void CCamera_Top::Set_Level(_uint iLevel)
{
}

void CCamera_Top::OnEnter(void* pArg)
{
	_vector vPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	m_vDownPosition = vPosition;
	m_vDownPosition.m128_f32[1] -= 7.5f;

	_vector vOrigin = _vector{ 0.f, m_vDownPosition.m128_f32[1], -58.f, 1.f };
	_vector vDir = XMVector3Normalize(m_vDownPosition - vOrigin);

	m_vBackPosition = vOrigin + vDir * m_fDistance;
}

void CCamera_Top::OnExit()
{
	m_isDown = true;
	m_isBack = false;

	m_fAccDownTime = 0.f;
	m_fAccBackTime = 0.f;
}

CCamera_Top* CCamera_Top::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Top* pInstance = new CCamera_Top(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CCamera_Top"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Top::Clone(void* pArg)
{
	CCamera_Top* pInstance = new CCamera_Top(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CCamera_Top"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Top::Free()
{
	__super::Free();

	if (true == m_isClone)
		Safe_Release(m_pEasing);
}

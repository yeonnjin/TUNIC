#include "Transform.h"
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CTransform::CTransform(const CTransform& rhs)
    : CComponent{ rhs }
    , m_WorldMatrix{ rhs.m_WorldMatrix }
{
}

void CTransform::Set_Scaled(_float fScaleX, _float fScaleY, _float fScaleZ)
{
    Set_State(STATE_RIGHT, XMVector3Normalize(Get_State_Vector(STATE_RIGHT)) * fScaleX);
    Set_State(STATE_UP, XMVector3Normalize(Get_State_Vector(STATE_UP)) * fScaleY);
    Set_State(STATE_LOOK, XMVector3Normalize(Get_State_Vector(STATE_LOOK)) * fScaleZ);
}

HRESULT CTransform::Initialize_Prototype()
{
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

    return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        TRANSFORM_DESC* pDesc = (TRANSFORM_DESC*)pArg;

        m_fSpeedPerSec = pDesc->fSpeedPerSec;
        m_fRotationPerSec = pDesc->fRotationPerSec;
    }

    return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
    return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

void CTransform::Go_Straight(_float fTimeDelta, class CNavigation* pNavigation)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vLook = Get_State_Vector(STATE_LOOK);

    vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

    if (nullptr == pNavigation)
        Set_State(STATE_POSITION, vPosition);
    else
    {
        if (true == pNavigation->isMove(vPosition))
            Set_State(STATE_POSITION, vPosition);
    }
}

void CTransform::Go_Backward(_float fTimeDelta)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vLook = Get_State_Vector(STATE_LOOK);

    vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vRight = Get_State_Vector(STATE_RIGHT);

    vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vRight = Get_State_Vector(STATE_RIGHT);

    vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

    Set_State(STATE_POSITION, vPosition);
}

void CTransform::Look_At(_fvector vAt)
{
    _vector vLook = vAt - Get_State_Vector(STATE_POSITION);
    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    _vector vUp = XMVector3Cross(vLook, vRight);

    _float3 vScaled = Get_Scaled();

    Set_State(STATE_RIGHT,  XMVector3Normalize(vRight) * vScaled.x);
    Set_State(STATE_UP,     XMVector3Normalize(vUp) * vScaled.y);
    Set_State(STATE_LOOK,   XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Look_At_For_LandOject(_fvector vAt)
{
    // XMMatrixDecompose();
    // : Transform 이 들고 있는 WorldMatrix 로 부터 값을 뽑아옴 (회전 정보 들고 오기 가능(쿼터니언))

    _vector vUp = Get_State_Vector(STATE_UP);
    _vector vLook = vAt - Get_State_Vector(STATE_POSITION);
    _vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
    vLook = XMVector3Cross(vRight, vUp);

    _float3 vScaled = Get_Scaled();

    Set_State(STATE_RIGHT,  XMVector3Normalize(vRight) * vScaled.x);
    Set_State(STATE_LOOK,   XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Move_To_Target(_fvector vTargetPos, _float fTimeDelta, _float fMinDistance)
{
    _vector vPosition = Get_State_Vector(STATE_POSITION);
    _vector vLook = vTargetPos - vPosition;

    // XMVector3Length() 하면 길이가 { 5.f, 5.f, 5.f, 5.f } 이런 식으로 채워짐 => 128을 32로 나눈 것 중 0번째
    if (fMinDistance <= XMVector3Length(vLook).m128_f32[0])
        vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
}

// 현재 각도에서 회전
void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
    _matrix RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

    for (size_t i = 0; i < STATE_POSITION; ++i)
    {
        Set_State(STATE(i), XMVector4Transform(Get_State_Vector((STATE)i), RotationMatrix));
    }
}

// 원점으로부터 회전
void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
    _float3 vScaled = Get_Scaled();

    _vector vState[] = {
        XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x,
        XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y,
        XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z
    };

    _matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

    for (size_t i = 0; i < STATE_POSITION; ++i)
    {
        // XMVector4Transform() : 벡터 x 변환 매트릭스 (Coord, Normal 같은 친구)
        Set_State(STATE(i), XMVector4Transform(vState[(STATE)i], RotationMatrix));
        /*Set_State(STATE(i), XMVector3TransformNormal(Get_State_Vector((STATE)i), RotationMatrix));*/
    }
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTransform* pInstance = new CTransform(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CTransform"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
    CTransform* pInstance = new CTransform(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CTransform"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTransform::Free()
{
    __super::Free();
}
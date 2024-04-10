#include "Target_Manager.h"

#include "RenderTarget.h"

CTarget_Manager::CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CTarget_Manager::Initialize()
{
    return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(const wstring& strRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    if (nullptr != Find_RenderTarget(strRenderTargetTag))
        return E_FAIL;

    CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iSizeX, iSizeY, ePixelFormat, vClearColor);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    m_RenderTargets.emplace(strRenderTargetTag, pRenderTarget);

    return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strRenderTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    list<CRenderTarget*>* pTargetList = Find_MRT(strMRTTag);
    if (nullptr == pTargetList)
    {
        list<CRenderTarget*> TargetList;
        TargetList.emplace_back(pRenderTarget);
        m_MRTs.emplace(strMRTTag, TargetList);
    }
    else
        pTargetList->emplace_back(pRenderTarget);

    Safe_AddRef(pRenderTarget);

    return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const wstring& strMRTTag)
{
    list<CRenderTarget*>* pTargetList = Find_MRT(strMRTTag);
    if (nullptr == pTargetList)
        return E_FAIL;

    m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pDSV);

    _uint iNumRenderTargets = { 0 };
    ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

    for (auto& pRenderTarget : *pTargetList)
        pRenderTargets[iNumRenderTargets++] = pRenderTarget->Get_RTV();

    m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets, m_pDSV);

    return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
    m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDSV);

    Safe_Release(m_pBackBufferRTV);
    Safe_Release(m_pDSV);

    return S_OK;
}

CRenderTarget* CTarget_Manager::Find_RenderTarget(const wstring& strRenderTargetTag)
{
    auto iter = m_RenderTargets.find(strRenderTargetTag);
    if (iter == m_RenderTargets.end())
        return nullptr;

    return iter->second;
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const wstring& strMRTTag)
{
    auto iter = m_MRTs.find(strMRTTag);
    if (iter == m_MRTs.end())
        return nullptr;

    return &iter->second;
}

CTarget_Manager* CTarget_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTarget_Manager* pInstance = new CTarget_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Create : CTarget_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTarget_Manager::Free()
{
    for (auto& Pair : m_MRTs)
    {
        for (auto& pRenderTarget : Pair.second)
            Safe_Release(pRenderTarget);
        Pair.second.clear();
    }
    m_MRTs.clear();

    for (auto& Pair : m_RenderTargets)
        Safe_Release(Pair.second);
    m_RenderTargets.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}

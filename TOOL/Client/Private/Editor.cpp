#include "stdafx.h"
#include "Editor.h"

#include "Test_Object.h"
#include "ImGuizmo.h"

#include <locale>
#include <codecvt>

#include <fstream>
//#include "Texture.h"

CEditor::CEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEditor::CEditor(const CEditor& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CEditor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEditor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Load_Model();

	return S_OK;
}

void CEditor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CEditor::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->New_Frame();

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);

	// To Do
	//Test();

	if (!m_isUsingGizmo && m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON, KEY_DOWN))
		Test_Picking();

	if (!m_isUsingGizmo && m_pGameInstance->Get_DIMouseState(DIMKS_RBUTTON, KEY_DOWN))
		Test_Mesh_Picking();

	if (m_pGameInstance->Get_DIKeyState(DIK_G, KEY_DOWN))
		m_isUsingGizmo = !m_isUsingGizmo;

	if (m_isUsingGizmo)
		Gizmo(m_pGizmoTransform);

	if (m_pGameInstance->Get_DIKeyState(DIK_V, KEY_DOWN))
		Save_File();

	if (m_pGameInstance->Get_DIKeyState(DIK_C, KEY_DOWN))
		Load_File();

	ImGui::EndFrame();
}

HRESULT CEditor::Render()
{	
	m_pGameInstance->Render();

	return S_OK;
}

HRESULT CEditor::Test_Picking()
{
	const CVIBuffer_Terrain* pTerrainBuffer = dynamic_cast<const CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_TOOL_MAP, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
	if (nullptr == pTerrainBuffer)
		return E_FAIL;

	const CTransform* pTerrainTransform = dynamic_cast<const CTransform*>(m_pGameInstance->Get_Component(LEVEL_TOOL_MAP, TEXT("Layer_Terrain"), TEXT("Com_Transform")));
	if (nullptr == pTerrainTransform)
		return E_FAIL;

	_float3 vPickingPos =  pTerrainBuffer->Compute_Picking(pTerrainTransform);

	if (!(0.f == vPickingPos.x && 0.f == vPickingPos.y && 0.f == vPickingPos.z))
	{
		CTest_Object::TEST_DESC tDesc = {};
		tDesc.vPosition = vPickingPos;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Test_Object"), &tDesc)))
			return E_FAIL;

		++m_iObjectCount;
	}
}

HRESULT CEditor::Test_Mesh_Picking()
{
	_uint iNumObjects = m_pGameInstance->Get_Object_Count(LEVEL_TOOL_MAP, TEXT("Layer_Object"));
	if (0 == iNumObjects)
		return E_FAIL;

	for (size_t i = 0; i < iNumObjects; ++i)
	{
		const CModel* pObjectModel = dynamic_cast<const CModel*>(m_pGameInstance->Get_Component(LEVEL_TOOL_MAP, TEXT("Layer_Object"), TEXT("Com_Model"), i));
		if (nullptr == pObjectModel)
			return E_FAIL;

		const CTransform* pObjectTransform = dynamic_cast<const CTransform*>(m_pGameInstance->Get_Component(LEVEL_TOOL_MAP, TEXT("Layer_Object"), TEXT("Com_Transform"), i));
		if (nullptr == pObjectTransform)
			return E_FAIL;

		if (true == pObjectModel->Check_Picking(pObjectTransform))
		{
			m_isUsingGizmo = true;
			m_pGizmoTransform = (CTransform*)pObjectTransform;
			return S_OK;
		}			
	}

	return S_OK;

}

void CEditor::Test()
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		static float fCharacterX = 0.0f;
		static float fCharacterY = 0.0f;
		static float fCharacterZ = 0.0f;

		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");

		ImGui::SliderFloat("float", &fCharacterX, 0.0f, 300.0f);
		ImGui::SliderFloat("float", &fCharacterY, 0.0f, 300.0f);
		ImGui::SliderFloat("float", &fCharacterZ, 0.0f, 1.0f);

		if (ImGui::Button("Save Me"))
			// File Save
			if (ImGui::Button("Close Me"))
				show_another_window = false;
		ImGui::End();
	}
}

HRESULT CEditor::Load_Model()
{
	// NON_ANIM_MODEL
	m_pGameInstance->Create_Prototype_Model(CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Models/Rock/"));

	// ANIM_MODEL
	m_pGameInstance->Create_Prototype_Model(CModel::TYPE_ANIM, TEXT("../Bin/Resources/Models/Object/"));

	return S_OK;
}

std::string WStringToString(const std::wstring& wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.to_bytes(wstr);
}

void CEditor::Tool_Model_List()
{
	//ImGui::Begin("Object_List");
	//
	//static _uint	iCurrentIndex = { 0 };
	//vector<wstring> strModel = *m_pGameInstance->Get_Model_List();
	////ImGui::ListBox("NON_ANIM", &iCurrentIndex,strModel, strModel->size(), 4);
	//
	//vector<std::string> strVec;
	//vector<const char*> cstrVec;
	//for (const auto& wstr : strModel)
	//{
	//	std::string str = WStringToString(wstr);
	//	strVec.push_back(str); // string 버전을 저장
	//}
	//
	//for (const auto& str : strVec)
	//{
	//	cstrVec.push_back(str.c_str()); // const char* 버전을 저장
	//}
	//
	////ImGui::ListBox("List Box", &iCurrentIndex, cstrVec.data(), cstrVec.size());
	//
	//ImGui::End();
}

void CEditor::Gizmo(CTransform* pTransform)
{
	ImGui::SetNextWindowPos(ImVec2(1024, 100), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(256, 256), ImGuiCond_Appearing);

	// create a window and insert the inspector
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(320, 340), ImGuiCond_Appearing);

	ImGui::Begin("Editor");

	if (ImGuizmo::IsUsing())
	{
		ImGui::Text("Using gizmo");
	}
	else
	{
		ImGui::Text(ImGuizmo::IsOver() ? "Over gizmo" : "");
		ImGui::SameLine();
		ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");
		ImGui::SameLine();
		ImGui::Text(ImGuizmo::IsOver(ImGuizmo::ROTATE) ? "Over rotate gizmo" : "");
		ImGui::SameLine();
		ImGui::Text(ImGuizmo::IsOver(ImGuizmo::SCALE) ? "Over scale gizmo" : "");
	}
	ImGui::Separator();

	m_pGameInstance->EditTransform(pTransform);

	ImGui::End();
	
}

HRESULT CEditor::Save_File()
{
	ofstream fout;
	fout.open("../Bin/Resources/Data/Model/Model1.dat", ios::out | ios::binary);

	_uint iObjectCount = m_pGameInstance->Get_Object_Count(LEVEL_TOOL_MAP, TEXT("Layer_Object"));
	if (0 == iObjectCount)
		return E_FAIL;

	// Object Count
	fout.write(reinterpret_cast<char*>(&iObjectCount), sizeof(_uint));

	// MODEL
	for (size_t i = 0; i < iObjectCount; ++i)
	{
		CModel* pObjectModel = (CModel*)(m_pGameInstance->Get_Component(LEVEL_TOOL_MAP, TEXT("Layer_Object"), TEXT("Com_Model"), i));
		if (nullptr == pObjectModel)
			return E_FAIL;

		MODELFILE* pModelFile = pObjectModel->Get_ModelFile();

		// Mesh	
		fout.write(reinterpret_cast<char*>(&pModelFile->iNumMeshes), sizeof(_uint));

		for (size_t i = 0; i < pModelFile->Meshes.size(); ++i)
		{
			MESHFILE* pMeshFile = pModelFile->Meshes[i]->Get_MeshFile();
			fout.write(reinterpret_cast<char*>(&pMeshFile->szName), sizeof(_char) * MAX_PATH);
			fout.write(reinterpret_cast<char*>(&pMeshFile->iMaterialIndex), sizeof(_uint));
			fout.write(reinterpret_cast<char*>(&pMeshFile->iNumFaces), sizeof(_uint));

			fout.write(reinterpret_cast<char*>(&pMeshFile->iNumBones), sizeof(_uint));
			for (size_t j = 0; j < pMeshFile->Bones.size(); ++j)
				fout.write(reinterpret_cast<char*>(&pMeshFile->Bones[j]), sizeof(_uint));

			fout.write(reinterpret_cast<char*>(&pMeshFile->iNumOffsetMatrices), sizeof(_uint));
			for (size_t j = 0; j < pMeshFile->OffsetMatrices.size(); ++j)
				fout.write(reinterpret_cast<char*>(&pMeshFile->OffsetMatrices[j]), sizeof(_float4x4));
		}

		// Material
		fout.write(reinterpret_cast<char*>(&pModelFile->iNumMaterials), sizeof(_uint));

		for (size_t i = 0; i < pModelFile->Materials.size(); ++i)
		{
			for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
			{
				CTexture* pTexture = pModelFile->Materials[i].MaterialTextures[j];
				if (nullptr == pTexture)
					continue;

				_tchar szPath[MAX_PATH] = { L"" };
				wsprintf(szPath, pTexture->Get_TextureFile());
				fout.write(reinterpret_cast<char*>(&szPath), sizeof(_tchar) * MAX_PATH);
			}
		}

		// Bone
		fout.write(reinterpret_cast<char*>(&pModelFile->TransformMatrix), sizeof(_float4x4));
		for (size_t i = 0; i < pModelFile->Bones.size(); ++i)
		{
			BONEFILE* pBoneFile = pModelFile->Bones[i]->Get_BoneFile();
			fout.write(reinterpret_cast<char*>(&pBoneFile->szName), sizeof(_char) * MAX_PATH);
			fout.write(reinterpret_cast<char*>(&pBoneFile->TransformationMatrix), sizeof(_float4x4));
			fout.write(reinterpret_cast<char*>(&pBoneFile->iParentBoneIndex), sizeof(_int));
		}

		// Animation
		fout.write(reinterpret_cast<char*>(&pModelFile->iNumAnimations), sizeof(_uint));
		fout.write(reinterpret_cast<char*>(&pModelFile->iCurrentAnimIndex), sizeof(_uint));
		fout.write(reinterpret_cast<char*>(&pModelFile->isLoop), sizeof(_bool));
		for (size_t i = 0; i < pModelFile->Animations.size(); ++i)
		{
			ANIMFILE* pAnimFile = pModelFile->Animations[i]->Get_AnimFile();
			fout.write(reinterpret_cast<char*>(&pAnimFile->szName), sizeof(_char) * MAX_PATH);
			fout.write(reinterpret_cast<char*>(&pAnimFile->fDuration), sizeof(_float));
			fout.write(reinterpret_cast<char*>(&pAnimFile->fTicksPerSecond), sizeof(_float));
			fout.write(reinterpret_cast<char*>(&pAnimFile->fTrackPosition), sizeof(_float));

			fout.write(reinterpret_cast<char*>(&pAnimFile->iNumChannels), sizeof(_uint));

			// Channel
			for (size_t j = 0; j < pAnimFile->Channels.size(); ++j)
			{
				CHANNELFILE* pChannelFile = pAnimFile->Channels[j]->Get_ChannelFile();
				fout.write(reinterpret_cast<char*>(&pChannelFile->szName), sizeof(_char) * MAX_PATH);
				fout.write(reinterpret_cast<char*>(&pChannelFile->iBoneIndex), sizeof(_int));
				fout.write(reinterpret_cast<char*>(&pChannelFile->iNumKeyFrames), sizeof(_uint));

				// KeyFrame
				for (size_t k = 0; k < pChannelFile->KeyFrames.size(); ++k)
					fout.write(reinterpret_cast<char*>(&pChannelFile->KeyFrames[k]), sizeof(KEYFRAME));
			}
		}

		fout.write(reinterpret_cast<char*>(&pModelFile->MeshBoneMatrices), sizeof(_float4x4) * 512);
	}

	fout.close();

	return S_OK;
}

HRESULT CEditor::Load_File()
{
	ifstream fin;
	fin.open("../Bin/Resources/Data/Model/Model1.dat", ios::in | ios::binary);

	_uint iObjectCount;

	fin.read(reinterpret_cast<char*>(&iObjectCount), sizeof(_uint));

	// MODEL
	for (size_t i = 0; i < iObjectCount; ++i)
	{		
		MODELFILE* pModelFile = new MODELFILE;

		// Mesh	
		fin.read(reinterpret_cast<char*>(&pModelFile->iNumMeshes), sizeof(_uint));

		for (size_t i = 0; i < pModelFile->iNumMeshes; ++i)
		{
			MESHFILE* pMeshFile = new MESHFILE;
			_char* szTest;/* = new _char[MAX_PATH];*/
			fin.read(reinterpret_cast<char*>(&szTest), sizeof(_char) * MAX_PATH);
			pMeshFile->szName = szTest;
			//strcpy_s(pMeshFile->szName, szTest);
			fin.read(reinterpret_cast<char*>(&pMeshFile->iMaterialIndex), sizeof(_uint));
			fin.read(reinterpret_cast<char*>(&pMeshFile->iNumFaces), sizeof(_uint));

			fin.read(reinterpret_cast<char*>(&pMeshFile->iNumBones), sizeof(_uint));
			for (size_t j = 0; j < pMeshFile->iNumBones; ++j)
			{
				_uint iBone = {};
				fin.read(reinterpret_cast<char*>(&iBone), sizeof(_uint));
				pMeshFile->Bones.push_back(iBone);
			}
			
			fin.read(reinterpret_cast<char*>(&pMeshFile->iNumOffsetMatrices), sizeof(_uint));
			for (size_t j = 0; j < pMeshFile->iNumOffsetMatrices; ++j)
			{
				_float4x4 OffsetMatrices = {};
				fin.read(reinterpret_cast<char*>(&OffsetMatrices), sizeof(_float4x4));
				pMeshFile->OffsetMatrices.push_back(OffsetMatrices);
			}
		}

		//// Material
		//fin.read(reinterpret_cast<char*>(&pModelFile->iNumMaterials), sizeof(_uint));

		//for (size_t i = 0; i < pModelFile->Materials.size(); ++i)
		//{
		//	for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		//	{
		//		CTexture* pTexture = pModelFile->Materials[i].MaterialTextures[j];
		//		if (nullptr == pTexture)
		//			continue;

		//		_tchar szPath[MAX_PATH] = { L"" };
		//		wsprintf(szPath, pTexture->Get_TextureFile());
		//		fin.read(reinterpret_cast<char*>(&szPath), sizeof(_tchar) * MAX_PATH);
		//	}
		//}

		//// Bone
		//fout.write(reinterpret_cast<char*>(&pModelFile->TransformMatrix), sizeof(_float4x4));
		//for (size_t i = 0; i < pModelFile->Bones.size(); ++i)
		//{
		//	BONEFILE* pBoneFile = pModelFile->Bones[i]->Get_BoneFile();
		//	fout.write(reinterpret_cast<char*>(&pBoneFile->szName), sizeof(_char) * MAX_PATH);
		//	fout.write(reinterpret_cast<char*>(&pBoneFile->TransformationMatrix), sizeof(_float4x4));
		//	fout.write(reinterpret_cast<char*>(&pBoneFile->iParentBoneIndex), sizeof(_int));
		//}

		//// Animation
		//fout.write(reinterpret_cast<char*>(&pModelFile->iNumAnimations), sizeof(_uint));
		//fout.write(reinterpret_cast<char*>(&pModelFile->iCurrentAnimIndex), sizeof(_uint));
		//fout.write(reinterpret_cast<char*>(&pModelFile->isLoop), sizeof(_bool));
		//for (size_t i = 0; i < pModelFile->Animations.size(); ++i)
		//{
		//	ANIMFILE* pAnimFile = pModelFile->Animations[i]->Get_AnimFile();
		//	fout.write(reinterpret_cast<char*>(&pAnimFile->szName), sizeof(_char) * MAX_PATH);
		//	fout.write(reinterpret_cast<char*>(&pAnimFile->fDuration), sizeof(_float));
		//	fout.write(reinterpret_cast<char*>(&pAnimFile->fTicksPerSecond), sizeof(_float));
		//	fout.write(reinterpret_cast<char*>(&pAnimFile->fTrackPosition), sizeof(_float));

		//	fout.write(reinterpret_cast<char*>(&pAnimFile->iNumChannels), sizeof(_uint));

		//	// Channel
		//	for (size_t j = 0; j < pAnimFile->Channels.size(); ++j)
		//	{
		//		CHANNELFILE* pChannelFile = pAnimFile->Channels[j]->Get_ChannelFile();
		//		fout.write(reinterpret_cast<char*>(&pChannelFile->szName), sizeof(_char) * MAX_PATH);
		//		fout.write(reinterpret_cast<char*>(&pChannelFile->iBoneIndex), sizeof(_int));
		//		fout.write(reinterpret_cast<char*>(&pChannelFile->iNumKeyFrames), sizeof(_uint));

		//		// KeyFrame
		//		for (size_t k = 0; k < pChannelFile->KeyFrames.size(); ++k)
		//			fout.write(reinterpret_cast<char*>(&pChannelFile->KeyFrames[k]), sizeof(KEYFRAME));
		//	}
	}
	

	fin.close();

	return S_OK;
}

CEditor* CEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEditor* pInstance = new CEditor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CEditor"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEditor::Clone(void* pArg)
{
	CEditor* pInstance = new CEditor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CEditor"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEditor::Free()
{
	__super::Free();

	Safe_Release(m_pGizmoTransform);
}

#include "Mesh.h"
#include "imgui/imgui.h"
#include "Surface.h"
#include <unordered_map>
#include <sstream>

namespace dx = DirectX;

ModelException::ModelException(int line, const char* file, std::string note) noexcept
	:CustomException(line, file),
	note(std::move(note))
{}

const char* ModelException::what() const noexcept
{
	std::ostringstream oss;
	oss << CustomException::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "Custom Model Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
	return note;
}

// Mesh
Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs)
{

	AddBind(std::make_shared<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	for (auto& pb : bindPtrs)
	{
		AddBind(std::move(pb));
	}

	AddBind(std::make_shared<Bind::TransformCbuf>(gfx, *this));
}
void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
}
DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}


// Node
Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in) noxnd
	:
	id(id),
	meshPtrs(std::move(meshPtrs)),
	name(name) 
{
	DirectX::XMStoreFloat4x4(&transform, transform_in);
	DirectX::XMStoreFloat4x4(&appliedTransform, dx::XMMatrixIdentity());
}
void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd
{
	const auto built =
		dx::XMLoadFloat4x4(&appliedTransform)*
		dx::XMLoadFloat4x4(&transform) *
		accumulatedTransform;

	for (const auto pm : meshPtrs)
	{
		pm->Draw(gfx, built);
	}
	for (const auto& pc : childPtrs)
	{
		pc->Draw(gfx, built);
	}
}
void Node::AddChild(std::unique_ptr<Node> pChild) noxnd
{
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}
//pass in the incremental Index and a optional int reference for the selected node
void Node::ShowTree( Node*& pSelectedNode) const noexcept
{
	//tree node sys is provided by Imgui
	//node Index serves as a id for gui tree nodes, increment through out the recursion
	//build up flags for current tree node
	// if there is no selected node, set selectedId to an impossible value
	const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow //open when you clicked the node
		 //current node to be selected if the currentindex==selectedindex match up, since the selectedinde is a optional value, we use .value_or(-1)
		| ((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);//if this node not have child, mark it as leaf
	
	//TreeNode > no difference between node have children and node not have
	//if the tree is expanded, recursively render all children
	//(void*)(intptr_t)currentNodeIndex cast if we want to use a int as the identifier for a node
	const auto expanded = 
		ImGui::TreeNodeEx((void*)(intptr_t)GetId(), node_flags, name.c_str()); //returns true if the node is expanded in UI
	
		//selectedIndex = ImGui::IsItemClicked() ? currentNodeIdex : selectedIndex;
	if (ImGui::IsItemClicked())//When this node is clicked, this checks if the LASTLY ADDED NODE is clicked or not.
	{
		//set the selectedIndex to current index, and selected node to current node
		pSelectedNode = const_cast<Node*>(this);//we using this const cast because this function is static and we are modify a out side value.
	}
	if(expanded)
	{ 
		for (const auto& pChild : childPtrs)
		{
			pChild->ShowTree(pSelectedNode); //call showtree on child recursively
		}
		// when complete render all the child node, goes up a level.
		ImGui::TreePop(); 
	}
}

void Node::SetAppliedtransform(DirectX::FXMMATRIX transform)noexcept
{
	dx::XMStoreFloat4x4(&appliedTransform, transform);
}

int Node::GetId() const noexcept
{
	return  id;
}

// ModelWindow
class ModelWindow //this class is onlt define in this cpp, so it will only be use in here
{
public:
	void Show(const char* windowName, const Node& root) noexcept
	{
		//window name defaul is Model
		windowName = windowName ? windowName : "Model";
		//need int to track node indices and selected node
		int nodeIndexTracker = 0;

		if (ImGui::Begin(windowName))
		{
			//split the window into 2 colums, TRUE means you want a border
			ImGui::Columns(2, nullptr, true);
			//recursive call the SHOWTREE function, it will handle how the treee Gui is presented
			root.ShowTree( pSelectedNode);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr)
			{
				auto& transform = transforms[pSelectedNode->GetId()];
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
			}
		}
		ImGui::End();
	}

	dx::XMMATRIX GetTransform() const noexcept
	{
		//generate transform based on GUI edited values from the unordered map using the optional index
		//**const auto& transform = transforms.at(*selectedIndex);
		assert(pSelectedNode != nullptr);
		const auto& transform = transforms.at(pSelectedNode->GetId());
		return dx::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
			dx::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}

	Node* GetSelectedNode() const noexcept
	{
		return pSelectedNode;
	}
private:
	std::optional<int> selectedIndex;
	Node* pSelectedNode;
	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	//Use unordered map tp map indeices to the transforms
	std::unordered_map<int, TransformParameters> transforms;
};
// Model
Model::Model(Graphics& gfx, const std::string fileName)
	:
	pWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals
	);

	if (pScene == nullptr)
	{
		throw ModelException(__LINE__, __FILE__, imp.GetErrorString());
	}

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials));
	}

	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode);
}
void Model::Draw(Graphics& gfx) const noxnd
{
	//get the edited value from GUI and apply to specific node
	if (auto node = pWindow->GetSelectedNode())
	{
		node->SetAppliedtransform( pWindow->GetTransform() );
	}
	//pRoot->Draw(gfx, pWindow->GetTransform());//recursion call to draw all mesh
	pRoot->Draw(gfx, dx::XMMatrixIdentity());
}

void Model::ShowWindow(const char* windowName) noexcept
{
	pWindow->Show(windowName, *pRoot);
}

Model::~Model() noexcept
{
}
// you have to put the empty Dtor inside Cpp file since we are doing 
//a forward decalcare a for class ModelWindow


std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial *const *pMaterials)
{
	namespace dx = DirectX;
	using Dvtx::VertexLayout;

	Dvtx::VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Texture2D)
	));
	/*
	const aiMaterial &material = *pMaterials[mesh.mMaterialIndex];
	for (unsigned int i = 0; i < material.mNumProperties; i++)
	{
		const aiMaterialProperty &prop = *material.mProperties[i];
	}*/

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])//load UV0 in, Max 8 UV sets supported
		);
	}

	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	std::vector<std::shared_ptr<Bind::Bindable>> bindablePtrs;

	bool hasSpecular = false;
	float shininess = 35.0f;
	//Bind Materials, mMaterialIndex means 1 mesh can only have 1 material, 
	//if a imported mesh have multiple material, it will be splited
	if (mesh.mMaterialIndex >= 0)//if a mesh don't have a material, index will be negtive
	{
		using namespace std::string_literals;
		const auto base = "Models\\nano_textured\\"s;

		auto &material = *pMaterials[mesh.mMaterialIndex];
		aiString texFileName;//Assimp special string object
		material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);
		bindablePtrs.push_back(std::make_shared<Bind::Texture>(gfx, Surface::FromFile(base + texFileName.C_Str())));
		//Check if the Specular Texture  exist
		if(material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
		{
			bindablePtrs.push_back(std::make_shared<Bind::Texture>(gfx, Surface::FromFile(base + texFileName.C_Str()), 1));
			hasSpecular = true;
		}
		else
		{
			material.Get(AI_MATKEY_SHININESS, shininess); // get shiniess value
		}
		bindablePtrs.push_back(std::make_shared<Bind::Sampler>(gfx));
	}

	bindablePtrs.push_back(std::make_shared<Bind::VertexBuffer>(gfx, vbuf));

	bindablePtrs.push_back(std::make_shared<Bind::IndexBuffer>(gfx, indices));

	auto pvs = std::make_shared<Bind::VertexShader>(gfx, std::wstring(L"PhongVS.cso"));
	auto pvsbc = pvs->GetBytecode();
	bindablePtrs.push_back(std::move(pvs));
	if (hasSpecular)
	{
		bindablePtrs.push_back(std::make_shared<Bind::PixelShader>(gfx, std::wstring(L"PhongPSSpecular.cso")));
	}
	else
	{
		bindablePtrs.push_back(std::make_shared<Bind::PixelShader>(gfx, std::wstring(L"PhongPS.cso")));
		
		struct PSMaterialConstant
		{
			//DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
			float specularIntensity = 0.8f;
			float specularPower;
			float padding[2];
		} pmc;
		pmc.specularPower = shininess;
		bindablePtrs.push_back(std::make_shared<Bind::PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));
	}
	

	bindablePtrs.push_back(std::make_shared<Bind::InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

	

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node)
{
	namespace dx = DirectX;
	const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	));//get the transpose and store in the transform in node mTransformation~~

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}
	// creating a node with name.C_Str(), smart ptr to mesh and transform
	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(),std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}

	return pNode;
}


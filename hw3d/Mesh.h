#pragma once
#include "DrawableBase.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ConditionalNoexcept.h"
#include <optional>

class ModelException : public CustomException
{
public:
	ModelException(int line, const char* file, std::string note) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetNote() const noexcept;
private:
	std::string note;
};

class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

class Node
{
	friend class Model;
public:
	//when construct every node, it will be named for indentification
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd;
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
	void SetAppliedtransform(DirectX::FXMMATRIX transform) noexcept;
	//handle the details for the tree Gui show up, it is recursive calling, so you only have to call it for the root node.
	//Pass in the incremental index, the optional index for the selected node, and a ref to ptr to the selected node
	void ShowTree(Node*& pSelectedNode) const noexcept;
	int GetId() const noexcept;
private:
	void AddChild(std::unique_ptr<Node> pChild) noxnd;
private:
	std::string name; //node name for indentification
	int id; //unique ID
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	DirectX::XMFLOAT4X4 transform;//loaded transform from the files 
	DirectX::XMFLOAT4X4 appliedTransform;//edited transform form the GUI
};

class Model
{
public:
	Model(Graphics& gfx, const std::string fileName);
	void Draw(Graphics& gfx) const noxnd;
	void ShowWindow(const char* WindowName = nullptr) noexcept;
	~Model() noexcept;
private:
	//we want to pass in the GFX, const reference to aiMesh and a array of aiMaterial for that mesh
	//mateials should be aiMaterial** is aptr to an array of ptrs(PP), Since the Ptr to array and the array of ptr to aiMaterial are all in assimp, it should be all const
	//So we are passing the mareial ptr loaded to this func as a const ptr to an array of const ptrs to aiMaterials!!!!!! 
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial *const *pMaterials);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node);
private:
	std::unique_ptr<Node> pRoot;//this model class only keep track of root node
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	std::unique_ptr<class ModelWindow> pWindow;//this forward declare the class
	//this class will only be this this cpp file so outside can't access it
};
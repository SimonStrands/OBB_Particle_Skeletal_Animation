#include "ParticleModel.h"
#include "Random.h"

void getTransforms(std::vector<DirectX::XMMATRIX>& v, Joint joint, DirectX::XMMATRIX parent) {

	//DirectX::XMMATRIX temp(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1);
	v.push_back(joint.localBindTransform);

	DirectX::XMMATRIX newParent = v[v.size() - 1];

	for (int i = 0; i < joint.GetChildJoints().size(); i++) {
		getTransforms(v, joint.GetChildJoints()[i], newParent);
	}
}

ParticleModel::ParticleModel(Graphics*& gfx, const std::string& filePath, vec3 position):
	positionMatris(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        position.x, position.y, position.z, 1.0f
    )
{
	//some kind of load file here
	//but now we just do this for debug
	std::vector<VolumetricVertex> vertecies;
	loadParticleModel(vertecies, "objects/testAnimation.fbx",animation, rootJoint);
	
	this->nrOfVertecies = (UINT)vertecies.size();
	this->VS = gfx->getVS()[4];
	this->GS = gfx->getGS()[0];
	this->PS = gfx->getPS()[4];
	this->inputLayout = gfx->getInputLayout()[2];
	loadCShader("ParticleSkeletalAnimationComputeShader.cso", gfx->getDevice(), cUpdate);
	if (!CreateTexture("objects/Particle/SphereDiff.png", gfx->getDevice(), gfx->getTexture(), diffuseTexture)) {
		std::cout << "cannot load particle texture" << std::endl;
	}
	if (!CreateTexture("objects/Particle/SphereNormal.jpg", gfx->getDevice(), gfx->getTexture(), normalMapTexture)) {
		std::cout << "cannot load particle normal" << std::endl;
	}
	CreateVertexConstBuffer(gfx, this->Vg_pConstantBuffer);
	//create UAV
	D3D11_BUFFER_DESC buffDesc;
	buffDesc.ByteWidth = sizeof(VolumetricVertex) * this->nrOfVertecies;
	buffDesc.Usage = D3D11_USAGE_DEFAULT;
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;//how does this bind with vertex Buffer?
	buffDesc.CPUAccessFlags = 0;
	buffDesc.StructureByteStride = 0;
	buffDesc.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertecies.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	
	if (FAILED(gfx->getDevice()->CreateBuffer(&buffDesc, &data, &vertexBuffer))) {
		printf("doesn't work create Buffer");
		return;
	}
	
	D3D11_UNORDERED_ACCESS_VIEW_DESC UavDesc;
	UavDesc.Format = DXGI_FORMAT_R32_FLOAT;
	UavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UavDesc.Buffer.FirstElement = 0;
	UavDesc.Buffer.NumElements = nrOfVertecies * 10;
	UavDesc.Buffer.Flags = 0;
	if (FAILED(gfx->getDevice()->CreateUnorderedAccessView(vertexBuffer, &UavDesc, &billUAV))) {
		printf("doesn't work create Buffer");
		return;
	}
	if(!CreateConstBuffer(gfx, this->computeShaderConstantBuffer, sizeof(ComputerShaderParticleModelConstBuffer), &CSConstBuffer)){
		std::cout << "stop" << std::endl;
	}
	this->CSConstBuffer.time.element = 0;

	std::vector<float> heightTest;
	for (int i = 0; i < animation.keyFrames.size(); i++) {
		heightTest.push_back(2);
	}
	std::vector<DirectX::XMMATRIX> trans;
	DirectX::XMMATRIX p(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	getTransforms(trans, rootJoint, p);

	OBBSkeleton = new OBBSkeletonDebug(trans, heightTest, gfx);
}

ParticleModel::~ParticleModel()
{
	vertexBuffer->Release();
	Vg_pConstantBuffer->Release();
	diffuseTexture->Release();
	normalMapTexture->Release();
	cUpdate->Release();
	billUAV->Release();
	computeShaderConstantBuffer->Release();
}

void ParticleModel::updateParticles(float dt, Graphics*& gfx)
{
	this->CSConstBuffer.dt.element = dt;
	this->CSConstBuffer.time.element += dt;

	//update computeshader const buffer
	D3D11_MAPPED_SUBRESOURCE resource;
	gfx->get_IMctx()->Map(computeShaderConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &CSConstBuffer, sizeof(ComputerShaderParticleModelConstBuffer));
	gfx->get_IMctx()->Unmap(computeShaderConstantBuffer, 0);
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	std::vector<DirectX::XMMATRIX> output;
	GetPose(animation,rootJoint,dt,output,rootJoint.localBindTransform,rootJoint.inverseBindTransform);
	//here

	//dispathc shit
	gfx->get_IMctx()->CSSetShader(cUpdate, nullptr, 0);

	gfx->get_IMctx()->CSSetConstantBuffers(0, 1, &computeShaderConstantBuffer);

	gfx->get_IMctx()->CSSetUnorderedAccessViews(0, 1, &billUAV, nullptr);

	gfx->get_IMctx()->Dispatch((UINT)nrOfVertecies/16 + 1, 1, 1);//calc how many groups we need beacuse right now I do not know

	//nulla unorderedaccesview
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	gfx->get_IMctx()->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
}

void ParticleModel::draw(Graphics*& gfx)
{
	UINT offset = 0;
	static UINT strid = sizeof(VolumetricVertex);

	gfx->get_IMctx()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	this->updateShaders(gfx);
	this->setShaders(gfx->get_IMctx());
	gfx->get_IMctx()->VSSetConstantBuffers(0, 1, &Vg_pConstantBuffer);
	gfx->get_IMctx()->PSSetShaderResources(0, 1, &diffuseTexture);
	gfx->get_IMctx()->PSSetShaderResources(1, 1, &normalMapTexture);
	gfx->get_IMctx()->IASetInputLayout(this->inputLayout);

	gfx->get_IMctx()->IASetVertexBuffers(0, 1, &this->vertexBuffer, &strid, &offset);
	gfx->get_IMctx()->Draw(nrOfVertecies, 0);
	OBBSkeleton->draw(gfx);
}

void ParticleModel::setShaders(ID3D11DeviceContext*& immediateContext)
{
	immediateContext->VSSetShader(this->VS, nullptr, 0);
	immediateContext->PSSetShader(this->PS, nullptr, 0);
	immediateContext->GSSetShader(this->GS, nullptr, 0);
	immediateContext->HSSetShader(nullptr, nullptr, 0);
	immediateContext->DSSetShader(nullptr, nullptr, 0);
}

void ParticleModel::updateShaders(Graphics*& gfx)
{
	gfx->getVertexconstbuffer()->transform.element = positionMatris;

	//changing vertex Shader cBuffer
	D3D11_MAPPED_SUBRESOURCE resource;

	gfx->get_IMctx()->Map(Vg_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, gfx->getVertexconstbuffer(), sizeof(Vcb));
	gfx->get_IMctx()->Unmap(Vg_pConstantBuffer, 0);
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
}

std::pair<int, float> ParticleModel::GetTimeFraction(std::vector<float>& times, float& dt)
{
	int segment = 0;
	while (dt > times[segment])
		segment++;
	float start = times[segment - 1];
	float end = times[segment];
	float frac = (dt - start) / (end - start);
	return { segment, frac };
}

using namespace DirectX;
void ParticleModel::GetPose(Animation& animation, Joint& skeleton, float dt, std::vector<XMMATRIX>& output, XMMATRIX& parentTransform, XMMATRIX& globalInverseTransform)
{
	KeyFrame& btt = animation.keyFrames[skeleton.GetName()];
	dt = fmod(dt, animation.length);
	std::pair<int, float> fp;
	//calculate interpolated position
	fp = GetTimeFraction(btt.positionTimestamps, dt);

	XMFLOAT3 position1 = btt.positions[fp.first - 1];
	XMFLOAT3 position2 = btt.positions[fp.first];

	XMVECTOR resultPos = XMVectorLerp(XMLoadFloat3(&position1), XMLoadFloat3(&position2), fp.second);
	XMFLOAT3 position;
	XMStoreFloat3(&position, resultPos);

	//calculate interpolated rotation
	fp = GetTimeFraction(btt.rotationTimestamps, dt);

	XMFLOAT4 rotation1 = btt.rotations[fp.first - 1];
	XMFLOAT4 rotation2 = btt.rotations[fp.first];

	XMVECTOR rotation = XMQuaternionSlerp(XMLoadFloat4(&rotation1), XMLoadFloat4(&rotation2), fp.second);

	//calculate interpolated scale
	fp = GetTimeFraction(btt.scaleTimestamps, dt);
	XMFLOAT3 scale1 = btt.scales[fp.first - 1];
	XMFLOAT3 scale2 = btt.scales[fp.first];

	XMVECTOR resultScale = XMVectorLerp(XMLoadFloat3(&scale1), XMLoadFloat3(&scale2), fp.second);
	XMFLOAT3 scale;
	XMStoreFloat3(&scale, resultScale);

	XMMATRIX posIdentity = XMMatrixIdentity();
	XMMATRIX scaleIdentity = XMMatrixIdentity();

	// calculate localTransform
	XMMATRIX transMat = XMMatrixTranslation(position.x, position.y, position.z);
	transMat = XMMatrixMultiply(posIdentity, transMat);


	//XMFLOAT4X4 rotationMat = glm::toMat4(rotation);
	XMMATRIX rotMat = XMMatrixRotationQuaternion(rotation);

	XMMATRIX scaleMat = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	scaleMat = XMMatrixMultiply(scaleIdentity, scaleMat);

	XMMATRIX localTransform = transMat * rotMat * scaleMat;
	XMMATRIX globalTransform = parentTransform * localTransform;

	output[skeleton.GetId()] = globalInverseTransform * globalTransform * skeleton.localBindTransform;
	//update values for children bones
	for (Joint& child : skeleton.GetChildJoints()) {
		GetPose(animation, child, dt, output, globalTransform, globalInverseTransform);
	}
	//std::cout << dt << " => " << position.x << ":" << position.y << ":" << position.z << ":" << std::endl;
}

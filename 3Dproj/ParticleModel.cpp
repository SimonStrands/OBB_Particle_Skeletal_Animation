#include "ParticleModel.h"

std::pair<unsigned int, float> ParticleModel::getTimeFraction(const std::vector<float>& times, float& dt) {
	unsigned int segment = 0;
	while (dt > times[segment]){
		segment++;
	}
	float start = times[segment - 1];
	float end = times[segment];
	float frac = (dt - start) / (end - start);
	return {segment, frac};
}

void ParticleModel::getPose(Bone& joint, const Animation& anim, float time, DirectX::XMMATRIX parentTransform){
	
	DirectX::XMMATRIX newParentTransform = DirectX::XMMatrixIdentity();

	float nTime = fmod(time, anim.length);
	if(nTime == 0){nTime += 0.00000001f;}
	KeyFrame bonePlacement = anim.keyFrames.find(joint.name)->second;

	std::pair<unsigned int, float> fp;

	fp = getTimeFraction(bonePlacement.positionTimestamps, nTime);
	DirectX::XMFLOAT3 pos1 = bonePlacement.positions[fp.first - 1];
	DirectX::XMFLOAT3 pos2 = bonePlacement.positions[fp.first];
	DirectX::XMVECTOR position = DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&pos1), DirectX::XMLoadFloat3(&pos2), fp.second);
	DirectX::XMMATRIX t = DirectX::XMMatrixTranslationFromVector(position);

	fp = getTimeFraction(bonePlacement.rotationTimestamps, nTime);
	DirectX::XMFLOAT4 rot1 = bonePlacement.rotations[fp.first - 1];
	DirectX::XMFLOAT4 rot2 = bonePlacement.rotations[fp.first];
	DirectX::XMVECTOR rotation = DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(&rot1), DirectX::XMLoadFloat4(&rot2), fp.second);
	DirectX::XMMATRIX r = DirectX::XMMatrixRotationQuaternion(rotation);

	fp = getTimeFraction(bonePlacement.scaleTimestamps, nTime);
	DirectX::XMFLOAT3 scale1 = bonePlacement.scales[fp.first - 1];
	DirectX::XMFLOAT3 scale2 = bonePlacement.scales[fp.first];
	DirectX::XMVECTOR scale = DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&scale1), DirectX::XMLoadFloat3(&scale2), fp.second);
	DirectX::XMMATRIX s = DirectX::XMMatrixScalingFromVector(scale);
	
	newParentTransform = parentTransform * DirectX::XMMatrixTranspose(s * r * t);

	this->SkeletonConstBufferConverter.Transformations.element[joint.id] = newParentTransform * joint.inverseBindPoseMatrix;

	for(int i = 0; i < joint.childJoints.size(); i++){
		getPose(joint.childJoints[i], anim, time, newParentTransform);
	}
}

int getNrOfBones(Bone joint){
	unsigned int x = 0;
	x += (unsigned int)joint.childJoints.size();
	for(int i = 0; i < joint.childJoints.size(); i++){
		
		x += getNrOfBones(joint.childJoints[i]);
	}
	return x;
}

ParticleModel::ParticleModel()
{
}

ParticleModel::ParticleModel(Graphics*& gfx, const std::string& filePath, vec3 position):
	positionMatris(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        position.x, position.y, position.z, 1.0f
    )
{
	init(gfx, filePath, position);
}

void ParticleModel::init(Graphics*& gfx, const std::string& filePath, vec3 position)
{
	this->positionMatris = DirectX::XMMATRIX(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        position.x, position.y, position.z, 1.0f
    );



	//some kind of load file here
	//but now we just do this for debug
	std::vector<VolumetricVertex> vertecies;
	std::map<int, IdAndWeight> idandWeight;
	loadParticleModel(vertecies, filePath, animation, rootJoint, idandWeight);

	hasAnimation = false;
	if(rootJoint.id != -1){
		hasAnimation = true;
	}
	if (hasAnimation) {
		getPose(rootJoint, animation, time);
	}

	if (!CreateConstBuffer(gfx, this->SkeletonConstBuffer, sizeof(SkeletonConstantBuffer), &SkeletonConstBufferConverter)) {
		std::cout << "failed to create const buffer" << std::endl;
	}
	
	for (int i = 0; i < vertecies.size(); i++)
	{

		DirectX::XMMATRIX boneTransform = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

		if (idandWeight[i].IDs.xyz.x > -0.5f)
		{
			boneTransform += this->SkeletonConstBufferConverter.Transformations.element[int(idandWeight[i].IDs.xyz.x)] * idandWeight[i].weights.xyz.x;
		}
		if (idandWeight[i].IDs.xyz.y > -0.5f)
		{
			boneTransform += this->SkeletonConstBufferConverter.Transformations.element[int(idandWeight[i].IDs.xyz.y)] * idandWeight[i].weights.xyz.y;
		}
		if (idandWeight[i].IDs.xyz.z > -0.5f)
		{
			boneTransform += this->SkeletonConstBufferConverter.Transformations.element[int(idandWeight[i].IDs.xyz.z)] * idandWeight[i].weights.xyz.z;
		}
		if (idandWeight[i].IDs.w > -0.5f)
		{
			boneTransform += this->SkeletonConstBufferConverter.Transformations.element[int(idandWeight[i].IDs.w)] * idandWeight[i].weights.w;
		}
	
		DirectX::XMFLOAT4 temp = DirectX::XMFLOAT4(vertecies[i].pos[0], vertecies[i].pos[1], vertecies[i].pos[2], 1.f);
		DirectX::XMVECTOR vPoint = DirectX::XMLoadFloat4(&temp);
		boneTransform = DirectX::XMMatrixTranspose(boneTransform);
		vPoint = DirectX::XMVector4Transform(vPoint, boneTransform);
		DirectX::XMStoreFloat4(&temp, vPoint);
		vertecies[i].pos[0] = temp.x;
		vertecies[i].pos[1] = temp.y;
		vertecies[i].pos[2] = temp.z;

	}

	//make it a multiple of 16 or can cause crashes
	if(vertecies.size() < 1){
		std::cout << "didn't get any vertecies" << std::endl;
		exit(-2);
	}
	while(vertecies.size() % 16 != 0){
		vertecies.push_back(vertecies[0]);
	}

	this->nrOfVertecies = (UINT)vertecies.size();

	this->VS = gfx->getVS()[4];
	this->GS = gfx->getGS()[0];
	this->PS = gfx->getPS()[4];
	this->PSShadow = gfx->getPS()[3];
	this->inputLayout = gfx->getInputLayout()[2];

	loadCShader("ParticleSkeletalAnimationComputeShader.cso", gfx->getDevice(), cUpdate);
	if (!CreateTexture("objects/Particle/SphereDiff.png", gfx->getDevice(), gfx->getTexture(), diffuseTexture)) {
		std::cout << "cannot load particle texture" << std::endl;
	}
	if (!CreateTexture("objects/Particle/SphereNormal.jpg", gfx->getDevice(), gfx->getTexture(), normalMapTexture)) {
		std::cout << "cannot load particle normal" << std::endl;
	}

	CreateVertexConstBuffer(gfx, this->Vg_pConstantBuffer);

	HRESULT hr;

	//create UAV
	D3D11_BUFFER_DESC buffDesc;
	buffDesc.ByteWidth = sizeof(VolumetricVertex) * this->nrOfVertecies;
	buffDesc.Usage = D3D11_USAGE_DEFAULT;
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
	buffDesc.CPUAccessFlags = 0;
	buffDesc.StructureByteStride = 0;
	buffDesc.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertecies.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	
	hr = gfx->getDevice()->CreateBuffer(&buffDesc, &data, &vertexBuffer);

	if (hr != S_OK) {
		printf("doesn't work create Buffer");
		return;
	}
	
	D3D11_UNORDERED_ACCESS_VIEW_DESC UavDesc;
	UavDesc.Format = DXGI_FORMAT_R32_FLOAT;
	UavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UavDesc.Buffer.FirstElement = 0;
#ifdef TRADITIONALSKELETALANIMATION
	UavDesc.Buffer.NumElements = nrOfVertecies * 18;
#else
	UavDesc.Buffer.NumElements = nrOfVertecies * 10;
#endif // DEBUG
	UavDesc.Buffer.Flags = 0;

	hr = gfx->getDevice()->CreateUnorderedAccessView(vertexBuffer, &UavDesc, &billUAV);

	if (hr != S_OK) {
		printf("doesn't work create Buffer");
		return;
	}



	#ifndef TRADITIONALSKELETALANIMATION
	//get bone orginal position
	
	std::vector<DirectX::XMFLOAT3> sizes; //x =height, y= Width, z=Depth
	std::ifstream sizesFile;

	//THEMODELCHANGE
	sizesFile.open("objects/obb_joint-boxessizes_1.txt");
	//sizesFile.open("objects/obb_joint-boxessizes_steave.txt");
	
	float x, y, z;
	while (!sizesFile.eof())
	{
		sizesFile >> x;
		sizesFile >> y;
		sizesFile >> z;
		sizes.push_back(DirectX::XMFLOAT3(x,y,z));
	}

	sizesFile.close();
	OBBSkeleton.init((unsigned int)sizes.size(), sizes, gfx);
	getHitBoxPosition(rootJoint, OBBSkeleton.getTransforms());

	#endif // DEBUG
}

ParticleModel::~ParticleModel()
{
	vertexBuffer->Release();
	Vg_pConstantBuffer->Release();
	diffuseTexture->Release();
	normalMapTexture->Release();
	cUpdate->Release();
	billUAV->Release();
	SkeletonConstBuffer->Release();
}

void ParticleModel::updateParticles(float dt, Graphics*& gfx)
{
	if(!this->hasAnimation){
		return;
	}
	if(getkey('P')){
		dt *= 20;
	}
	time += dt * animation.tick;
	
	getPose(rootJoint, animation, time);
	
	D3D11_MAPPED_SUBRESOURCE resource;
	gfx->get_IMctx()->Map(SkeletonConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &SkeletonConstBufferConverter, sizeof(SkeletonConstantBuffer));
	gfx->get_IMctx()->Unmap(SkeletonConstBuffer, 0);
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	
	gfx->get_IMctx()->VSSetConstantBuffers(1, 1, &SkeletonConstBuffer);
	
	#ifndef TRADITIONALSKELETALANIMATION
	
	OBBSkeleton.updateObbPosition(rootJoint, SkeletonConstBufferConverter);
	OBBSkeleton.update(gfx, dt);
	
	////dispathc shit
	gfx->get_IMctx()->CSSetShader(cUpdate, nullptr, 0);
	
	gfx->get_IMctx()->CSSetConstantBuffers(0, 1, &OBBSkeleton.getSkeletalTimeConstBuffer());
	gfx->get_IMctx()->CSSetConstantBuffers(1, 1, &OBBSkeleton.getSkeletalTransformConstBuffer());
	
	gfx->get_IMctx()->CSSetUnorderedAccessViews(0, 1, &billUAV, nullptr);
	
	gfx->get_IMctx()->Dispatch((UINT)nrOfVertecies / 16, 1, 1);//calc how many groups we need beacuse right now I do not know
	
	//nulla unorderedaccesview
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	gfx->get_IMctx()->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);

    #endif
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

	#ifndef TRADITIONALSKELETALANIMATION
	if(!this->hasAnimation){
		return;
	}
	if(getkey('O')){
		OBBSkeleton.draw(gfx);
	}
	
    #endif
}

void ParticleModel::drawShadow(Graphics*& gfx)
{
		UINT offset = 0;
	static UINT strid = sizeof(VolumetricVertex);

	gfx->get_IMctx()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	this->updateShaders(gfx);
	gfx->get_IMctx()->VSSetShader(this->VS, nullptr, 0);
	gfx->get_IMctx()->GSSetShader(this->GS, nullptr, 0);
	gfx->get_IMctx()->PSSetShader(this->PSShadow, nullptr, 0);
	gfx->get_IMctx()->PSSetShaderResources(0, 1, &diffuseTexture);
	gfx->get_IMctx()->VSSetConstantBuffers(0, 1, &Vg_pConstantBuffer);
	gfx->get_IMctx()->IASetInputLayout(this->inputLayout);

	gfx->get_IMctx()->IASetVertexBuffers(0, 1, &this->vertexBuffer, &strid, &offset);
	gfx->get_IMctx()->Draw(nrOfVertecies, 0);
}

#ifndef TRADITIONALSKELETALANIMATION
OBBSkeletonDebug& ParticleModel::getOBBSkeleton()
{
	return this->OBBSkeleton;
}
#endif // !TRADITIONALSKELETALANIMATION



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

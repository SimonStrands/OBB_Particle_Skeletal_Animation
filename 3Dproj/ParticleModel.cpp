#include "ParticleModel.h"
#include "Random.h"

std::pair<unsigned int, float> getTimeFraction(std::vector<float>& times, float& dt) {
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

	joint.boneMatrix = newParentTransform;

	DirectX::XMMATRIX finalTransform = newParentTransform * joint.inverseBindPoseMatrix;

	this->SkeletonConstBufferConverter.Transformations.element[joint.id] = finalTransform;

	for(int i = 0; i < joint.childJoints.size(); i++){
		getPose(joint.childJoints[i], anim, time, newParentTransform);
	}
}

int getNrOfBones(Bone joint){
	int x = 0;
	x += joint.childJoints.size();
	for(int i = 0; i < joint.childJoints.size(); i++){
		
		x += getNrOfBones(joint.childJoints[i]);
	}
	return x;
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
	loadParticleModel(vertecies, filePath, animation, rootJoint);
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
#ifdef TRADITIONALSKELETALANIMATION
	UavDesc.Buffer.NumElements = nrOfVertecies * 18;
#else
	UavDesc.Buffer.NumElements = nrOfVertecies * 10;
#endif // DEBUG
	UavDesc.Buffer.Flags = 0;
	if (FAILED(gfx->getDevice()->CreateUnorderedAccessView(vertexBuffer, &UavDesc, &billUAV))) {
		printf("doesn't work create Buffer");
		return;
	}
	if(!CreateConstBuffer(gfx, this->computeShaderConstantBuffer, sizeof(ComputerShaderParticleModelConstBuffer), &CSConstBuffer)){
		std::cout << "stop" << std::endl;
	}
	std::cout << sizeof(DirectX::XMMATRIX) << std::endl;
	if(!CreateConstBuffer(gfx, this->SkeletonConstBuffer, sizeof(SkeletonConstantBuffer), &SkeletonConstBufferConverter)){
		std::cout << "failed to create const buffer" << std::endl;
	}
	this->CSConstBuffer.time.element = 0;

	getPose(rootJoint, animation, time);

	#ifndef TRADITIONALSKELETALANIMATION
	//get bone orginal position
	std::vector<DirectX::XMFLOAT3> sizes; //x =height, y= Width, z=Depth


	//ROOT
	sizes[0].x = 0.2;
	sizes[0].y = 0.6;
	sizes[0].z = 0.3;

	sizes[1].x = 0.25;
	sizes[1].y = 0.2;
	sizes[1].z = 0.15;

	sizes[2].x = 0.25;
	sizes[2].y = 0.2;
	sizes[2].z = 0.15;

	sizes[3].x = 0.3;
	sizes[3].y = 0.3;
	sizes[3].z = 0.15;

	sizes[4].x = 0.2;
	sizes[4].y = 0.1;
	sizes[4].z = 0.15;

	sizes[5].x = 0.4;
	sizes[5].y = 0.4;
	sizes[5].z = 0.4;
	//left side
	//shoulder
	sizes[6].x = 0.4;
	sizes[6].y = 0.2;
	sizes[6].z = 0.2;
	//arm
	sizes[7].x = 0.6;
	sizes[7].y = 0.2;
	sizes[7].z = 0.2;
	//forearm
	sizes[8].x = 0.4;
	sizes[8].y = 0.15;
	sizes[8].z = 0.15;
	//hand
	sizes[9].x = 0.12;
	sizes[9].y = 0.12;
	sizes[9].z = 0.05;
	//thumb
	sizes[10].x = 0.1;
	sizes[10].y = 0.02;
	sizes[10].z = 0.02;
	//thumb
	sizes[11].x = 0.1;
	sizes[11].y = 0.02;
	sizes[11].z = 0.02;
	//thumb
	sizes[12].x = 0.1;
	sizes[12].y = 0.02;
	sizes[12].z = 0.02;
	//index
	sizes[13].x = 0.1;
	sizes[13].y = 0.02;
	sizes[13].z = 0.02;
	//index
	sizes[14].x = 0.1;
	sizes[14].y = 0.02;
	sizes[14].z = 0.02;
	//index
	sizes[15].x = 0.1;
	sizes[15].y = 0.02;
	sizes[15].z = 0.02;
	//middle
	sizes[16].x = 0.1;
	sizes[16].y = 0.02;
	sizes[16].z = 0.02;
	//middle
	sizes[17].x = 0.1;
	sizes[17].y = 0.02;
	sizes[17].z = 0.02;
	//middle
	sizes[18].x = 0.1;
	sizes[18].y = 0.02;
	sizes[18].z = 0.02;
	//ring
	sizes[19].x = 0.1;
	sizes[19].y = 0.02;
	sizes[19].z = 0.02;
	//ring
	sizes[20].x = 0.1;
	sizes[20].y = 0.02;
	sizes[20].z = 0.02;
	//ring
	sizes[21].x = 0.1;
	sizes[21].y = 0.02;
	sizes[21].z = 0.02;
	//pinky
	sizes[22].x = 0.1;
	sizes[22].y = 0.02;
	sizes[22].z = 0.02;
	//pinky
	sizes[23].x = 0.1;
	sizes[23].y = 0.02;
	sizes[23].z = 0.02;
	//pinky
	sizes[24].x = 0.1;
	sizes[24].y = 0.02;
	sizes[24].z = 0.02;
	//right side
	//shoulder
	sizes[25].x = 0.4;
	sizes[25].y = 0.2;
	sizes[25].z = 0.2;
	//arm
	sizes[26].x = 0.6;
	sizes[26].y = 0.2;
	sizes[26].z = 0.2;
	//forarm
	sizes[27].x = 0.4;
	sizes[27].y = 0.15;
	sizes[27].z = 0.15;
	//hand
	sizes[28].x = 0.12;
	sizes[28].y = 0.12;
	sizes[28].z = 0.05;
	//thumb
	sizes[29].x = 0.1;
	sizes[29].y = 0.02;
	sizes[29].z = 0.02;
	//thumb
	sizes[30].x = 0.1;
	sizes[30].y = 0.02;
	sizes[30].z = 0.02;
	//thumb
	sizes[31].x = 0.1;
	sizes[31].y = 0.02;
	sizes[31].z = 0.02;
	//index
	sizes[32].x = 0.1;
	sizes[32].y = 0.02;
	sizes[32].z = 0.02;
	//index
	sizes[33].x = 0.1;
	sizes[33].y = 0.02;
	sizes[33].z = 0.02;
	//index
	sizes[34].x = 0.1;
	sizes[34].y = 0.02;
	sizes[34].z = 0.02;
	//middle
	sizes[35].x = 0.1;
	sizes[35].y = 0.02;
	sizes[35].z = 0.02;
	//middle
	sizes[36].x = 0.1;
	sizes[36].y = 0.02;
	sizes[36].z = 0.02;
	//middle
	sizes[37].x = 0.1;
	sizes[37].y = 0.02;
	sizes[37].z = 0.02;
	//ring
	sizes[38].x = 0.1;
	sizes[38].y = 0.02;
	sizes[38].z = 0.02;
	//ring
	sizes[39].x = 0.1;
	sizes[39].y = 0.02;
	sizes[39].z = 0.02;
	//ring
	sizes[40].x = 0.1;
	sizes[40].y = 0.02;
	sizes[40].z = 0.02;
	//pinky
	sizes[41].x = 0.1;
	sizes[41].y = 0.02;
	sizes[41].z = 0.02;
	//pinky
	sizes[42].x = 0.1;
	sizes[42].y = 0.02;
	sizes[42].z = 0.02;
	//pinky
	sizes[43].x = 0.1;
	sizes[43].y = 0.02;
	sizes[43].z = 0.02;
	//Left upper leg
	sizes[44].x = 0.8;
	sizes[44].y = 0.2;
	sizes[44].z = 0.2;
	//left leg
	sizes[45].x = 1;
	sizes[45].y = 0.2;
	sizes[45].z = 0.2;
	//left foot
	sizes[46].x = 0.4;
	sizes[46].y = 0.2;
	sizes[46].z = 0.2;
	//left toebase
	sizes[47].x = 0.5;
	sizes[47].y = 0.2;
	sizes[47].z = 0.2;
	//right upper leg
	sizes[48].x = 0.8;
	sizes[48].y = 0.2;
	sizes[48].z = 0.2;
	//right leg
	sizes[49].x = 1;
	sizes[49].y = 0.2;
	sizes[49].z = 0.2;
	//right foot
	sizes[50].x = 0.4;
	sizes[50].y = 0.2;
	sizes[50].z = 0.2;
	//right toebase
	sizes[51].x = 0.5;
	sizes[51].y = 0.2;
	sizes[51].z = 0.2;
	OBBSkeleton = new OBBSkeletonDebug(sizes.size(), sizes, gfx);

	getHitBoxPosition(rootJoint, OBBSkeleton->getTransforms());




	

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
	computeShaderConstantBuffer->Release();
	SkeletonConstBuffer->Release();
	if(OBBSkeleton != nullptr){
		delete OBBSkeleton;
	}
}

void ParticleModel::updateParticles(float dt, Graphics*& gfx)
{
	time += dt * animation.tick;
	//time = 14.5f;
	getPose(rootJoint, animation, time);
	
	D3D11_MAPPED_SUBRESOURCE resource;
	gfx->get_IMctx()->Map(SkeletonConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &SkeletonConstBufferConverter, sizeof(SkeletonConstantBuffer));
	gfx->get_IMctx()->Unmap(SkeletonConstBuffer, 0);
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	
	gfx->get_IMctx()->VSSetConstantBuffers(1, 1, &SkeletonConstBuffer);

	#ifndef TRADITIONALSKELETALANIMATION
	//update computeshader const buffer
	this->CSConstBuffer.dt.element = dt;
	this->CSConstBuffer.time.element += dt;

	OBBSkeleton->updateObbPosition(rootJoint, SkeletonConstBufferConverter);

	//gfx->get_IMctx()->Map(computeShaderConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	//memcpy(resource.pData, &CSConstBuffer, sizeof(ComputerShaderParticleModelConstBuffer));
	//gfx->get_IMctx()->Unmap(computeShaderConstantBuffer, 0);
	//ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	//
	////dispathc shit
	//gfx->get_IMctx()->CSSetShader(cUpdate, nullptr, 0);
	//
	//gfx->get_IMctx()->CSSetConstantBuffers(0, 1, &computeShaderConstantBuffer);
	//
	//gfx->get_IMctx()->CSSetUnorderedAccessViews(0, 1, &billUAV, nullptr);
	//
	//gfx->get_IMctx()->Dispatch((UINT)nrOfVertecies/16 + 1, 1, 1);//calc how many groups we need beacuse right now I do not know
	//
	////nulla unorderedaccesview
	//ID3D11UnorderedAccessView* nullUAV = nullptr;
	//gfx->get_IMctx()->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
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
	OBBSkeleton->draw(gfx);
    #endif
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

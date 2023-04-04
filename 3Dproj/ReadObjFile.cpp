#include "ReadObjFile.h"
#include <DirectXMath.h>
#include "TrashCollector.h"
#include "ResourceManager.h"

std::vector<vec3> calcTangent(vertex* vex1, vertex* vex2, vertex* vex3)
{
	vertex vex[3]{ *vex1, *vex2, *vex3 };
	//get all vars we need
	DirectX::XMFLOAT3 e[2];
	DirectX::XMFLOAT2 deltaUV[2];
	vec3 v[3];
	vec2 uv[3];
#pragma region addVAndUV
	arrayToVec(vex[0].pos, v[0]);
	arrayToVec(vex[1].pos, v[1]);
	arrayToVec(vex[2].pos, v[2]);
	arrayToVec(vex[0].uv, uv[0]);
	arrayToVec(vex[1].uv, uv[1]);
	arrayToVec(vex[2].uv, uv[2]);
#pragma endregion addVAndUV

	vec3 dae = v[1] - v[0];
	e[0] = DirectX::XMFLOAT3(dae.x, dae.y, dae.z);
	dae = (v[2] - v[0]);
	e[1] = DirectX::XMFLOAT3(dae.x, dae.y, dae.z);
	//x = u    y = v
	deltaUV[0] = DirectX::XMFLOAT2(uv[1].x - uv[0].x, uv[1].y - uv[0].y);
	deltaUV[1] = DirectX::XMFLOAT2(uv[2].x - uv[0].x, uv[2].y - uv[0].y);
	DirectX::XMMATRIX a(//the delta
		deltaUV[1].y, -deltaUV[0].y, 0, 0,
		-deltaUV[1].x, deltaUV[0].x, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0);
	DirectX::XMMATRIX b(//the E:s
		e[0].x, e[0].y, e[0].z, 0,
		e[1].x, e[1].y, e[1].z, 0,
		0, 0, 0, 0,
		0, 0, 0, 0);
	float div = (deltaUV[0].x * deltaUV[1].y - deltaUV[0].y * deltaUV[1].x);
	if (div == 0) {
		div = 0.0001f;
	}
	DirectX::XMMATRIX res =
		1 / div
		* a * b;
	DirectX::XMFLOAT4X4 mtheReturn;
	DirectX::XMStoreFloat4x4(&mtheReturn, res);

	std::vector<vec3> theReturn;
	theReturn.push_back(vec3(mtheReturn._11, mtheReturn._12, mtheReturn._13));
	theReturn.push_back(vec3(mtheReturn._21, mtheReturn._22, mtheReturn._23));
	return theReturn;
}

void fixtangent(std::vector<vertex> &vertecies, std::vector<DWORD>& indecies)
{
	for (int i = 0; i < indecies.size();) {
		int a = indecies[i];
		int b = indecies[i+1];
		int c = indecies[i+2];
		std::vector<vec3> tanbi = calcTangent(&vertecies[indecies[i]], &vertecies[indecies[i+1]], &vertecies[indecies[i+2]]);
		tanbi[0].Normalize(); tanbi[1].Normalize();
		for (int p = 0; p < 3; p++) {
			vertecies[indecies[i+p]].fixtang(tanbi[0], tanbi[1]);
			//vertecies[i + p].fixtang(tanbi[0], tanbi[1]);
		}
		i += 3;
	}
}

bool getMatrialFromFile(std::string fileName, std::vector<Material*> &matrial, Graphics*& gfx, ResourceManager* rm)
{
	std::ifstream infile(fileName);
	std::string readWord;
	std::string trash;
	std::string mtlname;
	std::string mapName;
	int CTR = -1;
	
	if (!infile.is_open()) {
		printf("cannot open textures");
		return false;
	}
	bool done = false;
	while (std::getline(infile, readWord) && !done) {
		if (readWord.substr(0, 6) == "mtllib") {
			std::istringstream a;
			a.str(readWord);
			a >> trash >> mtlname;
			done = true;
		}
	}
	infile.close();
	infile.open("objects/" + mtlname);
	if (done && infile.is_open()) {
		while (std::getline(infile, readWord)) {
			if (readWord.substr(0, 6) == "newmtl") {
				CTR++;
				matrial.resize(CTR + 1, new Material(rm->getDef()));
				std::istringstream a;
				std::string b;
				a.str(readWord);
				a >> trash >> matrial[CTR]->name;
			}
			else if (readWord.substr(0, 2) == "Ns") {
				//do something
				std::istringstream a;
				std::string b;
				a.str(readWord);
				a >> trash >> matrial[CTR]->Ns;
			}
			else if (readWord.substr(0, 2) == "Ka") {
				//ambient
				std::istringstream a;
				a.str(readWord);
				a >> trash >> matrial[CTR]->Ka[0] >> matrial[CTR]->Ka[1] >> matrial[CTR]->Ka[2];
			}
			else if (readWord.substr(0, 2) == "Kd") {
				//deffuse
				std::istringstream a;
				a.str(readWord);
				a >> trash >> matrial[CTR]->Kd[0] >> matrial[CTR]->Kd[1] >> matrial[CTR]->Kd[2];
			}
			else if (readWord.substr(0, 2) == "Ks") {
				//deffuse
				std::istringstream a;
				a.str(readWord);
				a >> trash >>  matrial[CTR]->Ks[0] >> matrial[CTR]->Ks[1] >> matrial[CTR]->Ks[2];
			}
			else if (readWord.substr(0, 6) == "map_Ka") {
				//map_ambient
				std::istringstream a;
				a.str(readWord);
				a >> trash >> mapName;
				matrial[CTR]->loadTexture(mapName, gfx, 1, rm);
			}
			else if (readWord.substr(0, 6) == "map_Kd") {
				//map_diffuse
				std::istringstream a;
				a.str(readWord);
				a >> trash >> mapName;
				matrial[CTR]->loadTexture(mapName, gfx, 0, rm);
			}
			else if (readWord.substr(0, 6) == "map_Ks") {
				//map_Specular
				std::istringstream a;
				a.str(readWord);
				a >> trash >> mapName;
				matrial[CTR]->loadTexture(mapName, gfx, 2, rm);
			}
			else if (readWord.substr(0, 4) == "disp") {
				//map_Specular
				std::istringstream a;
				a.str(readWord);
				a >> trash >> mapName;
				matrial[CTR]->loadTexture(mapName, gfx, 4, rm);
			}
			else if (readWord.substr(0, 8) == "map_Bump") {
				//map_normal
				std::istringstream a;
				a.str(readWord);
				std::string b;
				bool done = false;
				while (a >> b && !done) {
					if (!(b.substr(0, 2) == "-bm" || b.substr(0, 8) == "map_Bump" || int(b[0]) < 58)) {
						done = true;
						matrial[CTR]->loadTexture(b, gfx, 3, rm);
					}
				}
			}
		}
	}
	else {
		//we didnt get what we wanted
		return false;
	}
	if (matrial.size() == 0) {
		matrial.push_back(new Material(rm->getDef()));
	}
	return true;
}

static int countdown = 0;
void createMesh(Graphics*& gfx, std::vector<MeshObj> &Meshes, std::vector<vertex> &vertecies, std::vector<DWORD> &indecies, Material* matrial) {
	fixtangent(vertecies, indecies);
	MeshObj a(gfx, vertecies, indecies, matrial);
	Meshes.push_back(a);
	if (matrial->flags.Maps[3]) {
		Meshes[Meshes.size() - 1].SetShaders(gfx->getVS()[0], gfx->getPS()[0]);
	}
	else {
		Meshes[Meshes.size() - 1].SetShaders(gfx->getVS()[0], gfx->getPS()[2]);
	}
	if (matrial->flags.Maps[4]) {
		Meshes[Meshes.size() - 1].SetShaders(gfx->getVS()[2]);
		Meshes[Meshes.size() - 1].SetShaders(gfx->getHS()[0], gfx->getDS()[0]);
	}
	vertecies.clear();
	indecies.clear();
}

void readFace(std::string readWord, std::vector<vertex> &vertecies, std::vector<DWORD> &indecies, std::vector<std::array<float, 3>> vPos, std::vector<std::array<float, 2>> vUv, std::vector<std::array<float, 4>> vNorm) {
	std::string sTemp2[4];
	std::string trash;
	char trashChar;
		std::stringstream a;
		a.str(readWord);
		a >> trash >> sTemp2[0] >> sTemp2[1] >> sTemp2[2] >> sTemp2[3];
		std::stringstream b;
		int indeciesVertecies, indeciesUV, indeciesNormals;
		
		if (sTemp2[3] != "") {
			
			for (int i = 0; i < 3; i++) {
				b.str(sTemp2[i]);
				b >> indeciesVertecies >> trashChar >> indeciesUV >> trashChar >> indeciesNormals;
				vertecies.push_back(vertex(vPos[indeciesVertecies - 1], vUv[indeciesUV - 1], vNorm[indeciesNormals - 1]));
				indecies.push_back((DWORD)((int)vertecies.size() - 1));
				b.clear();
			}
			b.str(sTemp2[3]);
			b >> indeciesVertecies >> trashChar >> indeciesUV >> trashChar >> indeciesNormals;
			//vertecies.push_back(vertecies[vertecies.size() - 1]); 
			indecies.push_back((DWORD)((int)vertecies.size() - 3));
			indecies.push_back((DWORD)((int)vertecies.size() - 1));
			vertecies.push_back(vertex(vPos[indeciesVertecies - 1], vUv[indeciesUV - 1], vNorm[indeciesNormals - 1]));
			indecies.push_back((DWORD)((int)vertecies.size() - 1));
			b.clear();
		}
		else {
			for (int i = 0; i < 3; i++) {
				b.str(sTemp2[i]);
				b >> indeciesVertecies >> trashChar >> indeciesUV >> trashChar >> indeciesNormals;
				vertecies.push_back(vertex(vPos[indeciesVertecies - 1], vUv[indeciesUV - 1], vNorm[indeciesNormals - 1]));
				indecies.push_back((DWORD)((int)vertecies.size() - 1));
				b.clear();
			}
		}
}

void getLowest(vec3 box[2], std::array<float, 3> vPos)
{
	//get the lowest
	if (box[0].x < vPos[0]) {
		box[0].x = vPos[0];
	}
	if (box[0].y < vPos[1]) {
		box[0].y = vPos[1];
	}
	if (box[0].z < vPos[2]) {
		box[0].z = vPos[2];
	}
	//get highest
	if (box[1].x > vPos[0]) {
		box[1].x = vPos[0];
	}
	if (box[1].y > vPos[1]) {
		box[1].y = vPos[1];
	}
	if (box[1].z > vPos[2]) {
		box[1].z = vPos[2];
	}
}

bool readObjFile(std::vector<MeshObj>& Meshes, std::string fileName, std::vector<Material*> &matrial, Graphics*& gfx, vec3 box[2])
{
	std::vector<DWORD> indecies;
	std::vector<vertex> vertecies;
	std::vector<std::array<float, 3>>vPos;
	std::vector<std::array<float, 2>>vUv;
	std::vector<std::array<float, 4>>vNorm;
	bool ff = false;
	int nrOfMeshesOffset = 0;
	int currentMatrial = 0;

	std::ifstream infile(fileName);
	std::string readWord;
	std::string trash;
	bool useOfG = false;
	bool first = true;
	if (!infile.is_open()) {
		return false;
	}

	while (std::getline(infile, readWord)) {
		if (ff && readWord.substr(0, 1) != "f") {
			ff = false;//we have read all the faces and now create a mesh
			createMesh(gfx, Meshes, vertecies, indecies, matrial[currentMatrial]);
			nrOfMeshesOffset--;
		}//read vertexes
		if (readWord.substr(0, 2) == "v ") {
			std::istringstream a;
			vPos.resize(vPos.size() + 1);
			a.str(readWord);
			a >> trash >> vPos[vPos.size() - 1][0] >> vPos[vPos.size() - 1][1] >> vPos[vPos.size() - 1][2];
			getLowest(box, vPos[vPos.size() - 1]);
		}
		else if (readWord.substr(0, 3) == "vt ") {
			std::istringstream a;
			a.str(readWord);
			vUv.resize(vUv.size() + 1);
			a >> trash >> vUv[vUv.size() - 1][0] >> vUv[vUv.size() - 1][1];
		}
		else if (readWord.substr(0, 3) == "vn ") {
			std::istringstream a;
			a.str(readWord);
			vNorm.resize(vNorm.size() + 1);
			a >> trash >> vNorm[vNorm.size() - 1][0] >> vNorm[vNorm.size() - 1][1] >> vNorm[vNorm.size() - 1][2];
			vNorm[vNorm.size() - 1][3] = 0;
		}//read face
		else if (readWord.substr(0, 1) == "f") {
			readFace(readWord, vertecies, indecies, vPos, vUv, vNorm);
			if (!ff) { nrOfMeshesOffset++; };
			ff = true;
		}
		else if (readWord.substr(0, 6) == "usemtl") {
			std::istringstream a;
			a.str(readWord);
			std::string mName;
			a >> trash >> mName;
			for (int i = 0; i < matrial.size(); i++) {
				if (matrial[i]->name == mName) {
					currentMatrial = i;
				}
			}
		}
	}
	if (nrOfMeshesOffset > 0) {
		createMesh(gfx, Meshes, vertecies, indecies, matrial[currentMatrial]);
	}
	else if (nrOfMeshesOffset < 0) {
		std::cout << "error" << std::endl;
		return false;
	}
	return true;
}
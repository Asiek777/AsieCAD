#include "milling.h"

#include "app.h"
#include "woodBlock.h"
#include "tinyxml2/tinyfiledialogs.h"
# define M_PI 3.14159265358979323846

float Milling::speed = 1.0f;
bool Milling::showPath = 1;

Milling::Milling(float _radius, bool _isFlat) :
	SceneObject("Milling"), radius(_radius), isFlat(_isFlat)
{
	shader = std::make_unique<Shader>("shaders/milling.vert", "shaders/phong.frag", "shaders/wood.geom");
	pathShader = std::make_unique<Shader>("shaders/torus.vert", "shaders/torus.frag");
	int width = 100, height = isFlat ? 2 : 50;
	std::vector<float> vertices;
	std::vector<unsigned> indices;
	for (int j = 0; j < width + 1; j++) {
		vertices.emplace_back(radius * sinf(j * 2.f * M_PI / width));
		vertices.emplace_back(5);
		vertices.emplace_back(radius * cosf(j * 2.f * M_PI / width));
	}
	if(isFlat)
		for (int j = 0; j < width + 1; j++) {
			vertices.emplace_back(radius * sinf(j * 2.f * M_PI / width));
			vertices.emplace_back(0);
			vertices.emplace_back(radius * cosf(j * 2.f * M_PI / width));
		}
	else
		for (int i = 0; i < height - 1; i++)
			for (int j = 0; j < width + 1; j++) {
				float r = cosf(i * M_PI / 2 / (height - 2)) * radius;
				vertices.emplace_back(r * sinf(j * 2.f * M_PI / width));
				vertices.emplace_back(radius * (1 - sinf(i * M_PI / 2 / (height - 2))));
				vertices.emplace_back(r * cosf(j * 2.f * M_PI / width));
			}
	
	for (int i = 0; i < height - 1; i++)
		for (int j = 0; j < width; j++) {
			indices.emplace_back(i * (width + 1) + j);
			indices.emplace_back((i + 1) * (width + 1) + j);
			indices.emplace_back(i * (width + 1) + j + 1);
			indices.emplace_back(i * (width + 1) + j + 1);
			indices.emplace_back((i + 1) * (width + 1) + j);
			indices.emplace_back((i + 1) * (width + 1) + j + 1);
		}
	for (int i = 1; i < width; i++) {
		indices.emplace_back(0);
		indices.emplace_back(i);
		indices.emplace_back(i + 1);
		indices.emplace_back((width + 1) * (height - 1) + 0);
		indices.emplace_back((width + 1) * (height - 1) + i);
		indices.emplace_back((width + 1) * (height - 1) + i + 1);
	};
	indexCount = indices.size();
	mesh = std::make_unique<MeshBuffer>(vertices, 1, indices);
}

Milling::~Milling()
{
}

void Milling::Render()
{
	shader->use();
	glBindVertexArray(mesh->GetVAO());
	shader->setVec3("color", glm::vec3(1.0f));
	shader->setMat4("viewProjection", viewProjection);
	shader->setMat4("model", glm::translate(glm::mat4(1), position));
	shader->setVec3("viewPos", App::GetCameraPos());
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	
	if (pathBuffer) {
		pathShader->use();
		glBindVertexArray(pathBuffer->GetVAO());
		pathShader->setVec3("color", glm::vec3(1.0f));
		pathShader->setMat4("viewProjection", viewProjection);
		pathShader->setMat4("model", glm::mat4(1));
		glDrawArrays(GL_LINE_STRIP, 0, path.size());
	}
	
	glBindVertexArray(0);
}

void Milling::Update(float dt)
{
	if (!pathBuffer)
		return;
	static float time = 0;
	const float delta = 0.03f;
	if (step < path.size() - 1)
		time += dt * speed;
	while (time > delta) {		
		float len = glm::distance(path[step], path[step + 1]);
		t += delta / len;
		if (t > 1) {
			step++;
			t = 0;
			if (step == path.size() - 1) {
				position = path[step];
				time = 0;
				break;
			}
		}
		position = t * path[step + 1] + (1 - t) * path[step];
		auto block = static_cast<WoodBlock*>(SceneObjects[1].get());
		block->UpdateWood(position, radius, isFlat);
		
		time -= delta;
	};
}

void Milling::MillingFileMenu()
{

	if (ImGui::CollapsingHeader("Milling options")) {
		if (ImGui::Button("Open paths file")) {
			char const* lTheOpenFileName;
			char const* lFilterPatterns[] = { "*.f*","*.k*" };
			lTheOpenFileName = tinyfd_openFileDialog(
				"Choose an XML file to read",
				"",
				2,
				lFilterPatterns,
				NULL,
				0);
			if (lTheOpenFileName) {
				ReadPaths(lTheOpenFileName);
			}
		}
		ImGui::DragFloat("Milling speed", &speed, 0.1f, 0, 100);
	}
}

void Milling::ReadPaths(std::string path)
{
	int length = path.length();
	char type = path[length - 3];
	bool isFlat = type ==  'f';
	float radius = atoi(path.substr(length - 2).c_str()) * 0.05f;
	auto milling = std::make_shared<Milling>(radius, isFlat);
	std::fstream fin(path);
	std::string move;
	while(!fin.eof()) {
		fin >> move;
		int offset[3];
		for (int i = 0; i < 3; i++)
			offset[i] = move.find('X' + i);
		std::string phrase[] = {
			move.substr(offset[0]+1, offset[1] - offset[0]) ,
			move.substr(offset[1]+1, offset[2] - offset[1]),
			move.substr(offset[2]+1) };
		glm::vec3 pos(atof(phrase[0].c_str()), atof(phrase[2].c_str()),
			-atof(phrase[1].c_str()));
		milling->path.emplace_back(pos / 10.f);
	}
	if (milling->path[milling->path.size() - 1] == glm::vec3(0))
		milling->path.erase(milling->path.end() - 1);
	milling->pathBuffer = std::make_unique<MeshBuffer>(
		MeshBuffer::Vec3ToFloats(milling->path));
	SceneObjects[2] = milling;
}

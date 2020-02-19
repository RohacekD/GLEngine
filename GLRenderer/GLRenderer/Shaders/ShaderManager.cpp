#include <GLRendererStdafx.h>

#include <GLRenderer/Shaders/ShaderManager.h>


#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

#include <pugixml.hpp>


namespace GLEngine {
namespace GLRenderer {
namespace Shaders {
//=================================================================================
const std::filesystem::path C_ShaderManager::s_ShadersFolder = "shaders/";

//=================================================================================
C_ShaderManager::C_ShaderManager()
	: m_Timeout(std::chrono::seconds(1))
	, m_LastUpdate(std::chrono::system_clock::now())
{

}

//=================================================================================
C_ShaderManager & C_ShaderManager::Instance()
{
	static C_ShaderManager    instance; // Guaranteed to be destroyed.
						  // Instantiated on first use.
	return instance;
}

//=================================================================================
void C_ShaderManager::Clear()
{
	std::cout << ShadersStatistics();
	m_Programs.clear();
	m_ActiveShader.reset();
}

//=================================================================================
void C_ShaderManager::Update()
{
	DeactivateShader();
#ifdef GL_ENGINE_DEBUG
	const auto currentTime = std::chrono::system_clock::now();
	if (m_LastUpdate + m_Timeout < currentTime) {
		m_ActiveShader.reset();
		for (auto& program : m_Programs) {
			if (!program.second->IsExpired())
			{
				continue;
			}

			C_ShaderCompiler compiler;
			try
			{
				*(program.second) = std::move(C_ShaderProgram(LoadProgram(std::filesystem::path(program.first), compiler)));
				program.second->SetPaths(compiler.GetTouchedFiles());
				Buffers::C_UniformBuffersManager::Instance().ProcessUBOBindingPoints(program.second);
			}
			catch (...)
			{
				CORE_LOG(E_Level::Error, E_Context::Render, "Unable to reload shader {} so we keep it outdated", program.first);
			}
		}
		m_LastUpdate = currentTime;
	}
#endif
}

//=================================================================================
C_ShaderManager::T_ShaderPtr C_ShaderManager::GetProgram(const std::string& name)
{
	if (ShaderLoaded(name)) {
		return m_Programs[name];
	}

	C_ShaderCompiler compiler;
	GLuint program = LoadProgram(name, compiler);
	if (program == 0) {
		return nullptr;
	}

	glObjectLabel(GL_PROGRAM, program, static_cast<GLsizei>(name.length()), name.c_str());

	T_ShaderPtr shaderProgram = std::make_shared<C_ShaderProgram>(program);
	shaderProgram->SetName(name);
	shaderProgram->SetPaths(compiler.GetTouchedFiles());

	Buffers::C_UniformBuffersManager::Instance().ProcessUBOBindingPoints(shaderProgram);

	m_Programs.insert({ name, shaderProgram });

	return shaderProgram;
}

//=================================================================================
bool C_ShaderManager::ShaderLoaded(const std::string & name)
{
	return m_Programs.find(name) != m_Programs.end();
}

//=================================================================================
void C_ShaderManager::ActivateShader(T_ShaderPtr shader)
{
	if (shader == nullptr) {
		throw 1;// just error
	}
	if (shader != m_ActiveShader || !m_ActiveShader->IsActive()) {
		m_ActiveShader = shader;
		m_ActiveShader->useProgram();
	}
}

//=================================================================================
void C_ShaderManager::DeactivateShader()
{
	// this is done because imGUI can change shader without letting us know
	if (m_ActiveShader) {
		m_ActiveShader->disableProgram();
		m_ActiveShader = nullptr;
	}
}

//=================================================================================
std::string C_ShaderManager::ShadersStatistics() const
{
	std::stringstream ss;
	ss << "Loaded shaders: " << m_Programs.size() << std::endl;
	for (auto& shader : m_Programs) {
		ss << shader.first << " used by " << shader.second.use_count() - 1 << std::endl;
		ss << "->\tHas stages:" << std::endl;
	}
	return ss.str();
}

//=================================================================================
bool C_ShaderManager::LoadDoc(pugi::xml_document & document, const std::filesystem::path& filename) const
{
	pugi::xml_parse_result result;

	if (filename.has_extension()) {
		result = document.load_file(filename.c_str());
		if (!result.status == pugi::status_ok) {
			std::filesystem::path path;
			path += s_ShadersFolder;
			path += filename;
			result = document.load_file(path.generic_string().c_str());
		}
		else {
			return true;
		}
	}
	else {
		auto path = filename;
		path.replace_extension("xml");
		return LoadDoc(document, path);
	}

	if (result.status == pugi::status_ok) return true;
	return false;
}

//=================================================================================
GLuint C_ShaderManager::LoadShader(const pugi::xml_node& node, C_ShaderCompiler& compiler) const
{
	GLuint shader = 0;
	std::string str;

	std::string stageAttribute = node.attribute("stage").value();

	int stage = GL_VERTEX_SHADER;
	if (stageAttribute == "vertex") stage = GL_VERTEX_SHADER;
	else if (stageAttribute == "fragment") stage = GL_FRAGMENT_SHADER;
	else if (stageAttribute == "geometry") stage = GL_GEOMETRY_SHADER;
	else if (stageAttribute == "compute") stage = GL_COMPUTE_SHADER;
	else if (stageAttribute == "tess-control") stage = GL_TESS_CONTROL_SHADER;
	else if (stageAttribute == "tess-evaluation") stage = GL_TESS_EVALUATION_SHADER;

	auto filename(s_ShadersFolder);
	filename += std::filesystem::path(node.first_child().value());

	if (!compiler.compileShader(shader, filename, stage)) {
		CORE_LOG(E_Level::Error, E_Context::Render, "--Compilation error");
		CORE_LOG(E_Level::Error, E_Context::Render, "{}", filename.generic_string());
		return 0;
	}
	glObjectLabel(GL_SHADER, shader, static_cast<GLsizei>(filename.generic_string().length()), filename.generic_string().c_str());
	return shader;
}

//=================================================================================
GLuint C_ShaderManager::LoadProgram(const std::filesystem::path& name, C_ShaderCompiler& compiler) const
{
	pugi::xml_document doc;

	if (!LoadDoc(doc, name)) {
		CORE_LOG(E_Level::Error, E_Context::Render, "Can't open config file for shader name: {}", name.generic_string());
		return 0;
	}

	std::vector<GLuint> shaders;

	for (auto& shader : doc.child("pipeline").children("shader")) {
		GLuint shaderStage = LoadShader(shader, compiler);
		if (shaderStage == 0) {
			return 0;
		}
		shaders.push_back(shaderStage);
	}

	GLuint program;
	if (!compiler.linkProgram(program, shaders)) {
		return false;
	}
	return program;
}

}
}
}
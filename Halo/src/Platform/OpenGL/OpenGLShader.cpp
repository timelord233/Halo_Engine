#include "hlpch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

namespace Halo {

	OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	int32_t OpenGLShader::GetUniformLocation(const std::string& name) const
	{
		int32_t result = glGetUniformLocation(m_RendererID, name.c_str());
		if (result == -1)
			HL_CORE_WARN("Could not find uniform '{0}' in shader", name);

		return result;
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = GetUniformLocation(name);
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		glUseProgram(m_RendererID);
		auto location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location != -1)
			glUniform1f(location, value);
		else
			HL_CORE_WARN("Uniform '{0}' not found!", name);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		glUseProgram(m_RendererID);
		auto location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location != -1)
			glUniform2f(location, value.x, value.y);
		else
			HL_CORE_WARN("Uniform '{0}' not found!", name);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		glUseProgram(m_RendererID);
		auto location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location != -1)
			glUniform3f(location, value.x, value.y, value.z);
		else
			HL_CORE_WARN("Uniform '{0}' not found!", name);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		glUseProgram(m_RendererID);
		auto location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location != -1)
			glUniform4f(location, value.x, value.y, value.z, value.w);
		else
			HL_CORE_WARN("Uniform '{0}' not found!", name);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = GetUniformLocation(name);
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		glUseProgram(m_RendererID);
		auto location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location != -1)
			glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)&matrix);
		else
			HL_CORE_WARN("Uniform '{0}' not found!", name);
	}

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;
		if (type == "compute");
			return GL_COMPUTE_SHADER;

		HL_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in, std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			HL_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			HL_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			HL_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		std::vector<GLuint> shaderRendererIDs;

		GLuint program = glCreateProgram();
		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			std::string source = kv.second;

			GLuint shaderRendererID = glCreateShader(type);
			const GLchar* sourceCstr = (const GLchar*)source.c_str();
			glShaderSource(shaderRendererID, 1, &sourceCstr, 0);

			glCompileShader(shaderRendererID);

			GLint isCompiled = 0;
			glGetShaderiv(shaderRendererID, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shaderRendererID, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shaderRendererID, maxLength, &maxLength, &infoLog[0]);

				HL_CORE_ERROR("Shader compilation failed:\n{0}", &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shaderRendererID);

				HL_CORE_ASSERT(false, "Failed");
			}

			shaderRendererIDs.push_back(shaderRendererID);
			glAttachShader(program, shaderRendererID);
		}

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
			HL_CORE_ERROR("Shader compilation failed:\n{0}", &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			for (auto id : shaderRendererIDs)
				glDeleteShader(id);
		}

		// Always detach shaders after a successful link.
		for (auto id : shaderRendererIDs)
			glDetachShader(program, id);

		m_RendererID = program;
	}

	void OpenGLShader::UploadUniformBuffer(const UniformBufferBase& uniformBuffer)
	{
		for (unsigned int i = 0; i < uniformBuffer.GetUniformCount(); i++)
		{
			const UniformDecl& decl = uniformBuffer.GetUniforms()[i];
			switch (decl.Type)
			{
			case UniformType::Int32:
			{
				const std::string& name = decl.Name;
				int value = *(int*)(uniformBuffer.GetBuffer() + decl.Offset);
				UploadUniformInt(name, value);
				break;
			}
			case UniformType::Float:
			{
				const std::string& name = decl.Name;
				float value = *(float*)(uniformBuffer.GetBuffer() + decl.Offset);
				UploadUniformFloat(name, value);
				break;
			}
			case UniformType::Float3:
			{
				const std::string& name = decl.Name;
				glm::vec3& values = *(glm::vec3*)(uniformBuffer.GetBuffer() + decl.Offset);
				UploadUniformFloat3(name, values);
				break;
			}
			case UniformType::Float4:
			{
				const std::string& name = decl.Name;
				glm::vec4& values = *(glm::vec4*)(uniformBuffer.GetBuffer() + decl.Offset);
				UploadUniformFloat4(name, values);
				break;
			}
			case UniformType::Matrix4x4:
			{
				const std::string& name = decl.Name;
				glm::mat4& values = *(glm::mat4*)(uniformBuffer.GetBuffer() + decl.Offset);
				UploadUniformMat4(name, values);
				break;
			}
			}
		}
	}

}
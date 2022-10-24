#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Halo {

	enum class UniformType
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Matrix3x3, Matrix4x4,
		Int32, Uint32
	};

	struct UniformDecl
	{
		UniformType Type;
		std::ptrdiff_t Offset;
		std::string Name;
	};

	struct UniformBuffer
	{
		// TODO: This currently represents a byte buffer that has been
		// packed with uniforms. This was primarily created for OpenGL,
		// and needs to be revisted for other rendering APIs. Furthermore,
		// this currently does not assume any alignment. This also has
		// nothing to do with GL uniform buffers, this is simply a CPU-side
		// buffer abstraction.
		std::byte* Buffer;
		std::vector<UniformDecl> Uniforms;
	};

	struct UniformBufferBase
	{
		virtual const std::byte* GetBuffer() const = 0;
		virtual const UniformDecl* GetUniforms() const = 0;
		virtual unsigned int GetUniformCount() const = 0;
	};

	template<unsigned int N, unsigned int U>
	struct UniformBufferDeclaration : public UniformBufferBase
	{
		std::byte Buffer[N];
		UniformDecl Uniforms[U];
		std::ptrdiff_t Cursor = 0;
		int Index = 0;

		virtual const std::byte* GetBuffer() const override { return Buffer; }
		virtual const UniformDecl* GetUniforms() const override { return Uniforms; }
		virtual unsigned int GetUniformCount() const { return U; }

		template<typename T>
		void Push(const std::string& name, const T& data) {}

		template<>
		void Push(const std::string& name, const int& data)
		{
			Uniforms[Index++] = { UniformType::Int32, Cursor, name };
			memcpy(Buffer + Cursor, &data, sizeof(int));
			Cursor += sizeof(int);
		}

		template<>
		void Push(const std::string& name, const float& data)
		{
			Uniforms[Index++] = { UniformType::Float, Cursor, name };
			memcpy(Buffer + Cursor, &data, sizeof(float));
			Cursor += sizeof(float);
		}

		template<>
		void Push(const std::string& name, const glm::vec3& data)
		{
			Uniforms[Index++] = { UniformType::Float3, Cursor, name };
			memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::vec3));
			Cursor += sizeof(glm::vec3);
		}

		template<>
		void Push(const std::string& name, const glm::vec4& data)
		{
			Uniforms[Index++] = { UniformType::Float4, Cursor, name };
			memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::vec4));
			Cursor += sizeof(glm::vec4);
		}

		template<>
		void Push(const std::string& name, const glm::mat4& data)
		{
			Uniforms[Index++] = { UniformType::Matrix4x4, Cursor, name };
			memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::mat4));
			Cursor += sizeof(glm::mat4);
		}

	};

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Shader* Create(const std::string & vertexSrc, const std::string & fragmentSrc);
		static Shader* Create(const std::string& filepath);

		virtual uint32_t GetRendererID() const = 0;
	private:
		uint32_t m_RendererID;
	};
}

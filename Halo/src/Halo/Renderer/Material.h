#pragma once

#include "Halo/Core/Base.h"

#include "Halo/Renderer/Shader.h"
#include "Halo/Renderer/Texture.h"

#include <unordered_set>

namespace Halo {

	class Material
	{
		friend class MaterialInstance;
	public:
		Material(const SharedPtr<Shader>& shader);
		virtual ~Material();

		void Bind() const;

		template <typename T>
		void Set(const std::string& name, const T& value)
		{
			auto decl = FindUniformDeclaration(name);
			HL_CORE_ASSERT(decl, "Could not find uniform with name '{0}'", name);
			auto& buffer = GetUniformBufferTarget(decl);
			buffer.Write((byte*)&value, decl->GetSize(), decl->GetOffset());

			for (auto mi : m_MaterialInstances)
				mi->OnMaterialValueUpdated(decl);
		}

		void Set(const std::string& name, const SharedPtr<Texture>& texture)
		{
			auto decl = FindResourceDeclaration(name);
			uint32_t slot = decl->GetRegister();
			if (m_Textures.size() <= slot)
				m_Textures.resize((size_t)slot + 1);
			m_Textures[slot] = texture;
		}

		void Set(const std::string& name, const SharedPtr<Texture2D>& texture)
		{
			Set(name, (const SharedPtr<Texture>&)texture);
		}

		void Set(const std::string& name, const SharedPtr<TextureCube>& texture)
		{
			Set(name, (const SharedPtr<Texture>&)texture);
		}
	private:
		void AllocateStorage();
		void OnShaderReloaded();
		void BindTextures() const;

		ShaderUniformDeclaration* FindUniformDeclaration(const std::string& name);
		ShaderResourceDeclaration* FindResourceDeclaration(const std::string& name);
		Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);
	private:
		SharedPtr<Shader> m_Shader;
		std::unordered_set<MaterialInstance*> m_MaterialInstances;

		Buffer m_VSUniformStorageBuffer;
		Buffer m_PSUniformStorageBuffer;
		std::vector<SharedPtr<Texture>> m_Textures;

		int32_t m_RenderFlags = 0;
	};

	class MaterialInstance
	{
		friend class Material;
	public:
		MaterialInstance(const SharedPtr<Material>& material);
		virtual ~MaterialInstance();

		template <typename T>
		void Set(const std::string& name, const T& value)
		{
			auto decl = m_Material->FindUniformDeclaration(name);
			HL_CORE_ASSERT(decl, "Could not find uniform with name '{0}'", name);
			auto& buffer = GetUniformBufferTarget(decl);
			buffer.Write((byte*)&value, decl->GetSize(), decl->GetOffset());

			m_OverriddenValues.insert(name);
		}

		void Set(const std::string& name, const SharedPtr<Texture>& texture)
		{
			auto decl = m_Material->FindResourceDeclaration(name);
			uint32_t slot = decl->GetRegister();
			if (m_Textures.size() <= slot)
				m_Textures.resize((size_t)slot + 1);
			m_Textures[slot] = texture;
		}

		void Set(const std::string& name, const SharedPtr<Texture2D>& texture)
		{
			Set(name, (const SharedPtr<Texture>&)texture);
		}

		void Set(const std::string& name, const SharedPtr<TextureCube>& texture)
		{
			Set(name, (const SharedPtr<Texture>&)texture);
		}

		void Bind() const;
	private:
		void AllocateStorage();
		void OnShaderReloaded();
		Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);
		void OnMaterialValueUpdated(ShaderUniformDeclaration* decl);
	private:
		SharedPtr<Material> m_Material;

		Buffer m_VSUniformStorageBuffer;
		Buffer m_PSUniformStorageBuffer;
		std::vector<SharedPtr<Texture>> m_Textures;

		// TODO: This is temporary; come up with a proper system to track overrides
		std::unordered_set<std::string> m_OverriddenValues;
	};

}
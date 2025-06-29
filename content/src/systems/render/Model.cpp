#include "systems/render/Model.hpp"
#include <utility>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <easylogging++.h>
#include <asset.hpp>
#include <assimp/Importer.hpp>
#include <command.hpp>
#include <containers.hpp>
#include <core/langdef.hpp>
#include <memory/destroyer.hpp>
#include <schedule.hpp>
#include "platform/path.hpp"
#include "systems/render/BufferObject.hpp"
#include "systems/render/Material.hpp"
#include "systems/render/Texture.hpp"
#include "systems/render/Vertex.hpp"
#include "systems/render/VertexArrayObject.hpp"

#define NAME_OF_ARG(x) #x

namespace atom::engine::systems::render {

Model::Model(const std::string& path) : m_Handle(), m_Path(path) {}

Model::Model(std::string&& path) noexcept : m_Handle(), m_Path(std::move(path)) {}

Model::Model(const Model& that) = default;

Model::Model(Model&& that) noexcept : m_Handle(that.m_Handle), m_Path(std::move(that.m_Path)) {}

Model& Model::operator=(const Model& that) {
    if (this == &that) {
        return *this;
    }

    if (m_Path == that.m_Path) {
        return *this;
    }

    reload(that.m_Path, that.m_Handle);

    return *this;
}

Model& Model::operator=(Model&& that) noexcept {
    if (this == &that) {
        return *this;
    }

    if (m_Path == that.m_Path) {
        return *this;
    }

    unload();

    m_Handle = std::exchange(that.m_Handle, 0);
    m_Path   = std::move(that.m_Path);

    return *this;
}

Model::~Model() noexcept = default;

[[nodiscard]] const std::string& Model::path() const noexcept { return m_Path; }

[[nodiscard]] constexpr auto Model::type() const noexcept -> ecs::asset_type {
    return ecs::asset_type::model;
}

ATOM_RELEASE_INLINE static void SetTexture(
    table<Texture>& table,
    library<Texture>& library,
    Texture& texture,
    const std::string& path,
    const char* textureType
) {
#if defined(ATOM_SHOW_MATERIAL_LOADING)
    LOG(INFO) << "Setting " << textureType << " at {" << path << "}";
#endif
    Texture tex(path);
    if (!table.contains(path)) {
        auto&& proxy = tex.load();
        auto handle  = library.install(std::move(proxy));
        table.emplace(path, handle);
        tex.set_handle(handle);
    }
    else {
        auto handle = table.at(path);
        tex.set_handle(handle);
    }
    texture = std::move(tex);
}

ATOM_RELEASE_INLINE static Mesh ProcessMesh(
    aiMesh* mesh, const aiScene* scene, const std::string& directory
) {
    const auto size_vector2f = 2 * sizeof(float);
    const auto size_vector3f = 3 * sizeof(float);

    const auto size = sizeof(Vertex) * mesh->mNumVertices;
    Mesh loading_mesh{};
    loading_mesh.vbo = VertexBufferObject(size);

    const std::string& dirconcat = directory + platform::sep;

    // load vertices in this mesh
    for (auto i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex{};
        std::memcpy(&vertex.position, &mesh->mVertices[i], size_vector3f);
        std::memcpy(&vertex.normal, &mesh->mNormals[i], size_vector3f);
        loading_mesh.vertices.emplace_back(vertex);
    }
    if (mesh->mTextureCoords[0]) {
        for (auto i = 0; i < mesh->mNumVertices; ++i) {
            std::memcpy(
                &loading_mesh.vertices[i].texCoords, &mesh->mTextureCoords[0][i], size_vector2f
            );
        }
    }
    if (mesh->mTangents) {
        for (auto i = 0; i < mesh->mNumVertices; ++i) {
            std::memcpy(&loading_mesh.vertices[i].tangent, &mesh->mTangents[i], size_vector3f);
        }
    }
    loading_mesh.vbo.set(loading_mesh.vertices.data());
    loading_mesh.vao.addAttributeForVertices(0, loading_mesh.vbo);

    // load indices in this mesh
    for (auto i = 0; i < mesh->mNumFaces; ++i) {
        auto& face = mesh->mFaces[i];

        // NOTE: this could be optimize in cpp23
        for (auto j = 0; j < face.mNumIndices; ++j) {
            loading_mesh.indices.emplace_back(face.mIndices[j]);
        }
    }
    // loading_mesh.ebo = ElementBufferObject(loading_mesh.indices.size());
    // loading_mesh.ebo.set(loading_mesh.indices.data());

    if (mesh->mMaterialIndex) [[likely]] {
        Material loading_material;
        auto* material = scene->mMaterials[mesh->mMaterialIndex];

        loading_material.name = material->GetName().C_Str();
#if defined(ATOM_SHOW_MATERIAL_LOADING)
        LOG(INFO) << "loading material: [" << loading_material.name << "]...";
#endif

        aiColor3D color3d;
        aiColor4D color4d;
        float floating{};

        if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color4d)) {
            loading_material.baseColorFactor =
                math::Vector4(color4d.r, color4d.g, color4d.b, color4d.a);
        }

        if (AI_SUCCESS == material->Get(AI_MATKEY_METALLIC_FACTOR, floating)) {
            loading_material.metallicFactor = floating;
        }

        if (AI_SUCCESS == material->Get(AI_MATKEY_ROUGHNESS_FACTOR, floating)) {
            loading_material.roughnessFactor = floating;
        }

        //
        // ambient factor
        //

        if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, color3d)) {
            loading_material.emissiveFactor = math::Vector3(color3d.r, color3d.g, color3d.b);
        }

        if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color3d)) {
            loading_material.ambientLight = math::Vector3(color3d.r, color3d.g, color3d.b);
        }

        // textures
        aiString path;

        auto& hub     = hub::instance();
        auto& library = hub.library<Texture>();
        auto& table   = hub.table<Texture>();
        if (AI_SUCCESS == material->GetTexture(aiTextureType_BASE_COLOR, 0, &path)) {
            SetTexture(
                table,
                library,
                loading_material.baseColorTexture,
                dirconcat + path.C_Str(),
                NAME_OF_ARG(aiTextureType_BASE_COLOR)
            );
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_AMBIENT, 0, &path)) {
            SetTexture(
                table,
                library,
                loading_material.ambientTexture,
                dirconcat + path.C_Str(),
                NAME_OF_ARG(aiTextureType_AMBIENT)
            );
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &path)) {
            SetTexture(
                table,
                library,
                loading_material.diffuseTexture,
                dirconcat + path.C_Str(),
                NAME_OF_ARG(aiTextureType_DIFFUSE)
            );
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_SPECULAR, 0, &path)) {
            SetTexture(
                table,
                library,
                loading_material.specularTexture,
                dirconcat + path.C_Str(),
                NAME_OF_ARG(aiTextureType_SPECULAR)
            );
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_EMISSIVE, 0, &path)) {
            SetTexture(
                table,
                library,
                loading_material.emissionTexture,
                dirconcat + path.C_Str(),
                NAME_OF_ARG(aiTextureType_EMISSIVE)
            );
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_METALNESS, 0, &path)) {
            SetTexture(
                table,
                library,
                loading_material.metallicTexture,
                dirconcat + path.C_Str(),
                NAME_OF_ARG(aiTextureType_METALNESS)
            );
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path)) {
            SetTexture(
                table,
                library,
                loading_material.roughnessTexture,
                dirconcat + path.C_Str(),
                NAME_OF_ARG(aiTextureType_DIFFUSE_ROUGHNESS)
            );
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path)) {
            SetTexture(
                table,
                library,
                loading_material.occlusionTexture,
                dirconcat + path.C_Str(),
                NAME_OF_ARG(aiTextureType_AMBIENT_OCCLUSION)
            );
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_NORMALS, 0, &path)) {
            SetTexture(
                table,
                library,
                loading_material.normalTexture,
                dirconcat + path.C_Str(),
                NAME_OF_ARG(aiTextureType_NORMALS)
            );
        }

        loading_mesh.materials.emplace_back(std::move(loading_material));
    }

    return loading_mesh;
}

ATOM_RELEASE_INLINE static void ProcessNodes(
    shared_ptr<Model::Proxy>& proxy,
    aiNode* node,
    const aiScene* scene,
    const std::string& directory
) {
    atom::queue<aiNode*> nodes;
    nodes.push(scene->mRootNode);
    while (!nodes.empty()) {
        auto* node = nodes.front();
        nodes.pop();
        for (auto i = 0; i < node->mNumMeshes; ++i) {
            auto* mesh = scene->mMeshes[node->mMeshes[i]];
            proxy->meshes.emplace_back(ProcessMesh(mesh, scene, directory));
        }

        for (auto i = 0; i < node->mNumChildren; ++i) {
            nodes.push(node->mChildren[i]);
        }
    }
}

auto Model::load() const -> shared_ptr<Model::Proxy> {

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        m_Path.c_str(),
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals |
            aiProcess_CalcTangentSpace // | aiProcess_JoinIdenticalVertices
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOG(ERROR) << "assimp error: " << importer.GetErrorString();
        return nullptr;
    }
    else {
        auto proxy           = std::make_shared<Model::Proxy>();
        const auto directory = m_Path.substr(0, m_Path.find_last_of("\\/"));
        ProcessNodes(proxy, scene->mRootNode, scene, directory);
        return proxy;
    }
}

void Model::unload() noexcept {
    if (m_Handle) [[likely]] {
        auto& hub     = hub::instance();
        auto& table   = hub.table<Model>();
        auto& library = hub.library<Model>();
        if (table.count(m_Path) == 1) {
            auto proxy = library.fetch(m_Handle);
            library.uninstall(m_Handle);
        }
        table.erase(m_Path);
        m_Handle = 0;
    }
}

void Model::reload(const key_type& key, const resource_handle handle) {
    auto& hub   = hub::instance();
    auto& table = hub.table<Model>();

    if (m_Handle) {
        auto& library = hub.library<Model>();
        if (table.count(m_Path) == 1) {
            auto proxy = library.fetch(m_Handle);
            library.uninstall(m_Handle);
        }
        table.erase(m_Path);
        m_Handle = 0;
    }

    if (handle) {
        table.emplace(key, handle);
    }

    m_Path   = key;
    m_Handle = handle;
}

void Model::draw(library<Model>& library, ShaderProgram& program) {
    auto proxy = library.fetch(m_Handle);

    if (!proxy->visibility) {
        return;
    }

    auto& meshes = proxy->meshes;
    for (auto& mesh : meshes) {
        if (mesh.visibility) {
            mesh.draw(program);
        }
    }
}

} // namespace atom::engine::systems::render

#undef FirstOf

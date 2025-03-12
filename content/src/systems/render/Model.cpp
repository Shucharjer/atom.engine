#include "systems/render/Model.hpp"
#include <utility>
#include "pch.hpp"

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
#include "systems/render/Material.hpp"
#include "systems/render/Texture.hpp"
#include "systems/render/Vertex.hpp"

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
    table<Texture>& table, library<Texture>& library, Texture& texture, const std::string& path
) {
    LOG(INFO) << "Setting texture at {" << path << "}";
    Texture tex(path);
    if (!table.contains(path)) {
        LOG(INFO) << "Texture not exist, creating...";
        auto&& proxy = tex.load();
        auto handle  = library.install(std::move(proxy));
        table.emplace(path, handle);
        tex.set_handle(handle);
    }
    else {
        LOG(INFO) << "Texture has already existed";
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

    Mesh loading_mesh{};

    const std::string& dirconcat = directory + platform::sep;

    // load vertices in this mesh
    for (auto i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex{};
        std::memcpy(&vertex.position, &mesh->mVertices[i], size_vector3f);
        std::memcpy(&vertex.normal, &mesh->mNormals[i], size_vector3f);
        if (mesh->mTextureCoords[0]) {
            std::memcpy(&vertex.texCoords, &mesh->mTextureCoords[0][i], size_vector2f);
        }
        loading_mesh.vertices.emplace_back(vertex);
    }

    // load indices in this mesh
    for (auto i = 0; i < mesh->mNumFaces; ++i) {
        auto& face = mesh->mFaces[i];

        // NOTE: this could be optimize in cpp23
        for (auto j = 0; j < face.mNumIndices; ++j) {
            loading_mesh.indices.emplace_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex) {
        Material loading_material;
        auto* material = scene->mMaterials[mesh->mMaterialIndex];

        const std::string& name = material->GetName().data;
        loading_material.name   = material->GetName().C_Str();
        LOG(INFO) << "loading material: [" << name << "]...";

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
            SetTexture(table, library, loading_material.baseColorTexture, dirconcat + path.C_Str());
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_AMBIENT, 0, &path)) {
            SetTexture(table, library, loading_material.ambientTexture, dirconcat + path.C_Str());
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &path)) {
            SetTexture(table, library, loading_material.diffuseTexture, dirconcat + path.C_Str());
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_SPECULAR, 0, &path)) {
            SetTexture(table, library, loading_material.specularTexture, dirconcat + path.C_Str());
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_EMISSIVE, 0, &path)) {
            SetTexture(table, library, loading_material.emissiveTexture, dirconcat + path.C_Str());
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_METALNESS, 0, &path)) {
            SetTexture(table, library, loading_material.metallicTexture, dirconcat + path.C_Str());
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path)) {
            SetTexture(table, library, loading_material.roughnessTexture, dirconcat + path.C_Str());
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path)) {
            SetTexture(table, library, loading_material.occlusionTexture, dirconcat + path.C_Str());
        }

        if (AI_SUCCESS == material->GetTexture(aiTextureType_NORMALS, 0, &path)) {
            SetTexture(table, library, loading_material.normalTexture, dirconcat + path.C_Str());
        }

        loading_mesh.materials.emplace_back(loading_material);
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
        m_Path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_OptimizeMeshes
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

} // namespace atom::engine::systems::render

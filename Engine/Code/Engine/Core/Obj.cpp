#include "Engine/Core/Obj.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Profiling/ProfileLogScope.hpp"

#include <numeric>
#include <sstream>
#include <string>

namespace FileUtils {

//Run only as an asynchronous operation highly recommended.
Obj::Obj(std::filesystem::path filepath) noexcept {
    namespace FS = std::filesystem;
    if(!FS::exists(filepath)) {
        filepath = FS::canonical(filepath);
    }
    filepath.make_preferred();
    if(!Load(filepath)) {
        const auto ss = std::string{"Obj: "} + filepath.string() + " failed to load.";
        ERROR_AND_DIE(ss.c_str());
    }
}

bool Obj::Load(std::filesystem::path filepath) noexcept {
    PROFILE_LOG_SCOPE_FUNCTION();

    namespace FS = std::filesystem;
    bool not_exist = !FS::exists(filepath);
    std::string valid_extension = ".obj";
    bool not_obj = StringUtils::ToLowerCase(filepath.extension().string()) != valid_extension;
    bool invalid = not_exist || not_obj;
    if(invalid) {
        DebuggerPrintf("%s is not a .obj file.\n", filepath.string().c_str());
        return false;
    }
    filepath = FS::canonical(filepath);
    filepath.make_preferred();
    return Parse(filepath);
}

bool Obj::Save(std::filesystem::path filepath) noexcept {
    PROFILE_LOG_SCOPE_FUNCTION();

    namespace FS = std::filesystem;
    filepath.make_preferred();

    _is_saving = true;
    std::ostringstream buffer;
    buffer << std::fixed << std::setprecision(6);
    for(auto& v : _verts) {
        buffer << "v " << v.x << ' ' << v.y << ' ' << v.z << '\n';
    }
    for(auto& v : _normals) {
        //v.Normalize();
        buffer << "vn " << v.x << ' ' << v.y << ' ' << v.z << '\n';
    }
    for(auto& v : _tex_coords) {
        buffer << "vt " << v.x << ' ' << v.y << ' ' << v.z << '\n';
    }
    bool has_vn = !_normals.empty();
    bool has_vt = !_tex_coords.empty();
    bool has_neither = !has_vt && !has_vn;
    for(auto iter = std::begin(_face_idxs); iter != std::end(_face_idxs); /* DO NOTHING */) {
        auto value1 = std::get<0>(*iter);
        auto value2 = std::get<1>(*iter);
        auto value3 = std::get<2>(*iter);
        buffer << "f ";
        buffer << (1 + value1);
        if(!has_neither) {
            buffer << '/';
            if(has_vt) {
                buffer << (1 + value2);
            }
            buffer << '/';
            if(has_vn) {
                buffer << (1 + value3);
            }
        }
        ++iter;
        value1 = std::get<0>(*iter);
        value2 = std::get<1>(*iter);
        value3 = std::get<2>(*iter);
        buffer << ' ';
        buffer << (1 + value1);
        if(!has_neither) {
            buffer << '/';
            if(has_vt) {
                buffer << (1 + value2);
            }
            buffer << '/';
            if(has_vn) {
                buffer << (1 + value3);
            }
        }
        ++iter;
        value1 = std::get<0>(*iter);
        value2 = std::get<1>(*iter);
        value3 = std::get<2>(*iter);
        buffer << ' ';
        buffer << (1 + value1);
        if(!has_neither) {
            buffer << '/';
            if(has_vt) {
                buffer << (1 + value2);
            }
            buffer << '/';
            if(has_vn) {
                buffer << (1 + value3);
            }
        }
        buffer << '\n';
        ++iter;
    }
    buffer.flush();
    if(FileUtils::WriteBufferToFile(buffer.str().data(), buffer.str().size(), filepath)) {
        _is_saved = true;
        _is_saving = false;
        return true;
    }
    _is_saving = false;
    _is_saved = false;
    return false;
}

bool Obj::IsLoaded() const noexcept {
    return _is_loaded;
}

bool Obj::IsLoading() const noexcept {
    return _is_loading;
}

bool Obj::IsSaving() const noexcept {
    return _is_saving;
}

bool Obj::IsSaved() const noexcept {
    return _is_saved;
}

const std::vector<Vertex3D>& Obj::GetVbo() const noexcept {
    return _vbo;
}

const std::vector<unsigned int>& Obj::GetIbo() const noexcept {
    return _ibo;
}

void Obj::Unload() noexcept {
    _vbo.clear();
    _vbo.shrink_to_fit();
    _ibo.clear();
    _ibo.shrink_to_fit();
    _is_loaded = false;
    _is_loading = false;
    _is_saved = false;
    _is_saving = false;
    _verts.clear();
    _verts.shrink_to_fit();
    _tex_coords.clear();
    _tex_coords.shrink_to_fit();
    _normals.clear();
    _normals.shrink_to_fit();
    _face_idxs.clear();
    _face_idxs.shrink_to_fit();
}

bool Obj::Parse(const std::filesystem::path& filepath) noexcept {
    PROFILE_LOG_SCOPE_FUNCTION();
    _verts.clear();
    _tex_coords.clear();
    _normals.clear();
    _vbo.clear();
    _ibo.clear();
    _face_idxs.clear();

    _is_loaded = false;
    _is_saving = false;
    _is_saved = false;
    _is_loading = true;
    if(auto buffer = FileUtils::ReadBinaryBufferFromFile(filepath)) {
        if(std::stringstream ss{}; ss.write(reinterpret_cast<const char*>(buffer->data()), buffer->size())) {
            buffer->clear();
            buffer->shrink_to_fit();
            ss.clear();
            ss.seekg(ss.beg);
            ss.seekp(ss.beg);
            std::string cur_line{};
            std::size_t vert_count{};
            unsigned long long line_index = 0;
            while(std::getline(ss, cur_line, '\n')) {
                if(StringUtils::StartsWith(cur_line, "v ")) {
                    ++vert_count;
                }
            }
            ss.clear();
            ss.seekg(ss.beg);
            ss.seekp(ss.beg);
            _verts.reserve(vert_count);
            _vbo.resize(vert_count);
            while(std::getline(ss, cur_line, '\n')) {
                ++line_index;
                cur_line = cur_line.substr(0, cur_line.find_first_of('#'));
                if(cur_line.empty()) {
                    continue;
                }
                cur_line = StringUtils::TrimWhitespace(cur_line);
                if(StringUtils::StartsWith(cur_line, "mtllib ")) {
                    continue;
                } else if(StringUtils::StartsWith(cur_line, "usemtl ")) {
                    continue;
                } else if(StringUtils::StartsWith(cur_line, "v ")) {
                    auto elems = StringUtils::Split(std::string{std::begin(cur_line) + 2, std::end(cur_line)}, ' ');
                    std::string v_str = {"["};
                    v_str += StringUtils::Join(elems, ',');
                    switch(elems.size()) {
                    case 4: /* DO NOTHING */ break;
                    case 3: v_str += ",1.0"; break;
                    case 2: v_str += ",0.0,1.0"; break;
                    case 1: v_str += ",0.0,0.0,1.0"; break;
                    default: PrintErrorToDebugger(filepath, "vertex", line_index); return false;
                    }
                    v_str += "]";
                    Vector4 v(v_str);
                    v.CalcHomogeneous();
                    _verts.emplace_back(v);
                } else if(StringUtils::StartsWith(cur_line, "vt ")) {
                    auto elems = StringUtils::Split(std::string{std::begin(cur_line) + 3, std::end(cur_line)}, ' ');
                    std::string v_str = {"["};
                    v_str += StringUtils::Join(elems, ',');
                    switch(elems.size()) {
                    case 3: /* DO NOTHING */ break;
                    case 2: v_str += ",0.0"; break;
                    case 1: v_str += ",0.0,0.0"; break;
                    default: PrintErrorToDebugger(filepath, "texture coordinate", line_index); return false;
                    }
                    v_str += "]";
                    _tex_coords.emplace_back(v_str);
                } else if(StringUtils::StartsWith(cur_line, "vn ")) {
                    auto elems = StringUtils::Split(std::string{std::begin(cur_line) + 3, std::end(cur_line)}, ' ');
                    std::string v_str = {"["};
                    v_str += StringUtils::Join(elems, ',');
                    if(elems.size() != 3) {
                        PrintErrorToDebugger(filepath, "vertex normal", line_index);
                        return false;
                    }
                    v_str += "]";
                    _normals.emplace_back(v_str);
                } else if(StringUtils::StartsWith(cur_line, "f ")) {
                    if(cur_line.find('-') != std::string::npos) {
                        DebuggerPrintf("OBJ implementation does not support relative reference numbers!\n");
                        PrintErrorToDebugger(filepath, "face index", line_index);
                        return false;
                    }
                    auto tris = StringUtils::Split(std::string{std::begin(cur_line) + 2, std::end(cur_line)}, ' ');
                    if(tris.size() != 3) {
                        DebuggerPrintf("OBJ implementation does not support non-triangle faces!\n");
                        PrintErrorToDebugger(filepath, "face triplet", line_index);
                        return false;
                    }
                    for(auto& t : tris) {
                        auto elems = StringUtils::Split(t, '/', false);
                        Vertex3D vertex{};
                        decltype(_face_idxs)::value_type face{};
                        auto elem_count = elems.size();
                        std::size_t cur_vbo_index = 0;
                        for(auto i = 0u; i < elem_count; ++i) {
                            switch(i) {
                            case 0:
                                if(!elems[0].empty()) {
                                    std::size_t cur_v = std::stoul(elems[0]);
                                    cur_vbo_index = cur_v - 1;
                                    std::get<0>(face) = cur_vbo_index;
                                    vertex.position = _verts[cur_vbo_index];
                                    _ibo.push_back(static_cast<unsigned int>(cur_vbo_index));
                                } else {
                                    std::get<0>(face) = static_cast<std::size_t>(-1);
                                }
                                break;
                            case 1:
                                if(!elems[1].empty()) {
                                    std::size_t cur_vt = std::stoul(elems[1]);
                                    std::get<1>(face) = cur_vt;
                                    vertex.texcoords = Vector2{_tex_coords[cur_vt - 1]};
                                } else {
                                    std::get<1>(face) = static_cast<std::size_t>(-1);
                                }
                                break;
                            case 2:
                                if(!elems[2].empty()) {
                                    std::size_t cur_vn = std::stoul(elems[2]);
                                    std::get<2>(face) = cur_vn - 1;
                                    vertex.normal = _normals[cur_vn - 1];
                                } else {
                                    std::get<2>(face) = static_cast<std::size_t>(-1);
                                }
                                break;
                            default: break;
                            }
                        }
                        _vbo[cur_vbo_index] = vertex;
                        _face_idxs.emplace_back(face);
                    }
                } else {
                    /* DO NOTHING */
                }
            }
            _ibo.shrink_to_fit();
            _is_loaded = true;
            _is_loading = false;
            return true;
        }
    }
    _is_loading = false;
    return false;
}

void Obj::PrintErrorToDebugger(std::filesystem::path filepath, const std::string& elementType, unsigned long long line_index) const noexcept {
    namespace FS = std::filesystem;
    filepath = FS::canonical(filepath);
    filepath.make_preferred();
    DebuggerPrintf("%s(%lld): Invalid %s\n", filepath.string().c_str(), line_index, elementType.c_str());
}

} // namespace FileUtils
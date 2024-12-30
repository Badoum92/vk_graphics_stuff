#include "vox.h"

#include <string.h>
#include <stdlib.h>

#include "bul/bul.h"
#include "bul/file.h"
#include "bul/allocators/scope_allocator.h"
#include "bul/log.h"

// clang-format off
static uint32_t default_palette[256] = {
    0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
    0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
    0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
    0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
    0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
    0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
    0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
    0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
    0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
    0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
    0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
    0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
    0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
    0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
    0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
    0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
};
// clang-format on

#if 0
static bool chunk_id_eq(const vox_chunk_id* chunk, const char id[4])
{
    return strncmp(chunk->id, id, 4) == 0;
}

static size_t chunk_size(const vox_chunk_id* chunk)
{
    return sizeof(vox_chunk_id) + chunk->n_bytes + chunk->n_children_bytes;
}

static const vox_chunk_id* next_chunk(const vox_chunk_id* chunk)
{
    return bul::ptr_offset<vox_chunk_id*>(chunk, chunk_size(chunk));
}

static const void* chunk_data(const vox_chunk_id* chunk)
{
    return bul::ptr_offset(chunk, sizeof(vox_chunk_id));
}

static bool end_of_data(const vox_chunk_id* chunk, const std::vector<uint8_t>& bytes)
{
    return chunk == (void*)(bytes.data() + bytes.size());
}

static std::string parse_string(const void* data)
{
    uint32_t size = *(uint32_t*)data;
    return std::string(bul::ptr_offset<char*>(data, sizeof(uint32_t)), size);
}

static std::unordered_map<std::string, std::string> parse_dict(const void* data)
{
    uint32_t n_pairs = *(uint32_t*)data;
    std::unordered_map<std::string, std::string> dict(n_pairs);
    data = bul::ptr_offset(data, sizeof(uint32_t));
    for (size_t i = 0; i < n_pairs; ++i)
    {
        std::string key = parse_string(data);
        data = bul::ptr_offset(data, sizeof(uint32_t) + key.size());
        std::string val = parse_string(data);
        data = bul::ptr_offset(data, sizeof(uint32_t) + val.size());
        dict.emplace(key, val);
    }
    return dict;
}

Model::Model(const std::string_view path)
{
    load(path);
}

void Model::load(const std::string_view path)
{
    chunks.clear();
    ASSERT(false);
    // bul::read_file(path.data(), bytes_);

    auto vox_header = (Header*)(bytes_.data());
    if (strncmp(vox_header->magic, "VOX ", 4) != 0)
    {
        std::cerr << "Invalid vox model " << path << "\n";
        bytes_.resize(0);
        return;
    }

    std::memcpy(palette.data(), default_palette, 256 * sizeof(uint32_t));

    const vox_chunk_id* main_chunk = (vox_chunk_id*)bul::ptr_offset(vox_header, sizeof(Header));
    const vox_chunk_id* chunk = (vox_chunk_id*)bul::ptr_offset(main_chunk, sizeof(vox_chunk_id));

    for (; !end_of_data(chunk, bytes_); chunk = next_chunk(chunk))
    {
        if (chunk_id_eq(chunk, "PACK"))
        {
            parse_pack(chunk);
        }
        else if (chunk_id_eq(chunk, "SIZE"))
        {
            parse_size(chunk);
        }
        else if (chunk_id_eq(chunk, "XYZI"))
        {
            parse_xyzi(chunk);
        }
        else if (chunk_id_eq(chunk, "RGBA"))
        {
            parse_rgba(chunk);
        }
        else if (chunk_id_eq(chunk, "MATL"))
        {
            parse_matl(chunk);
        }
    }
}

void Model::parse_pack(const vox_chunk_id* chunk)
{
    chunks.reserve(*(uint32_t*)chunk_data(chunk));
}

void Model::parse_size(const vox_chunk_id* chunk)
{
    auto& c = chunks.emplace_back();
    c.size = (SIZE*)chunk_data(chunk);
}

void Model::parse_xyzi(const vox_chunk_id* chunk)
{
    uint32_t n_voxels = *(uint32_t*)chunk_data(chunk);
    XYZI* xyzi = bul::ptr_offset<XYZI*>(chunk_data(chunk), sizeof(uint32_t));
    auto& c = chunks.back();
    c.n_voxels = n_voxels;
    c.xyzi = xyzi;
}

void Model::parse_ntrn(const vox_chunk_id*)
{}

void Model::parse_ngrp(const vox_chunk_id*)
{}

void Model::parse_nshp(const vox_chunk_id*)
{}

void Model::parse_layr(const vox_chunk_id*)
{}

void Model::parse_rgba(const vox_chunk_id* chunk)
{
    std::memcpy(palette.data() + 1, chunk_data(chunk), 255 * sizeof(uint32_t));
    palette[0] = {0, 0, 0, 0};
}

void Model::parse_matl(const vox_chunk_id* chunk)
{
    uint32_t id = *(uint32_t*)chunk_data(chunk);
    if (id > 255)
    {
        return;
    }
    const auto& dict = parse_dict(bul::ptr_offset(chunk_data(chunk), sizeof(uint32_t)));

    /* std::cout << "MATL: " << id << "\n";
    for (const auto& [key, val] : dict)
    {
        std::cout << key << ": " << val << "\n";
    }
    std::cout << "\n"; */

    MATL& matl = materials[id];
    if (dict.contains("_type"))
    {
        const auto& type = dict.at("_type");
        if (type == "_emit")
        {
            matl.type = EMISSIVE;
            if (dict.contains("_emit"))
            {
                matl.emit = std::stof(dict.at("_emit"));
            }
            if (dict.contains("_flux"))
            {
                matl.flux = std::stof(dict.at("_flux"));
            }
        }
        else if (type == "_metal")
        {
            matl.type = METAL;
            if (dict.contains("_metal"))
            {
                matl.metal = std::stof(dict.at("_metal"));
            }
            if (dict.contains("_rough"))
            {
                matl.rough = std::stof(dict.at("_rough"));
            }
            /* if (dict.contains("_ior"))
            {
                matl.ior = std::stof(dict.at("_ior"));
            } */
        }
        else if (type == "_glass")
        {
            matl.type = GLASS;
            if (dict.contains("_trans"))
            {
                matl.trans = std::stof(dict.at("_trans"));
            }
            if (dict.contains("_ior"))
            {
                matl.ior = std::stof(dict.at("_ior"));
            }
        }
    }
}

void Model::parse_robj(const vox_chunk_id*)
{}

void Model::parse_rcam(const vox_chunk_id*)
{}

void Model::parse_note(const vox_chunk_id*)
{}
#endif

struct vox_header
{
    char magic[4];
    int32_t version;
};

struct vox_chunk_header
{
    char id[4];
    int32_t n_bytes;
    int32_t n_children_bytes;
};

struct vox_size
{
    uint32_t x;
    uint32_t z;
    uint32_t y;
};

struct vox_xyzi
{
    uint8_t x;
    uint8_t z;
    uint8_t y;
    uint8_t i;
};

struct vox_str
{
    uint32_t size;
    const char* str;
};

static vox_header* parse_vox_header(uint8_t** data)
{
    vox_header* header = (vox_header*)*data;
    *data += sizeof(vox_header);
    return header;
}

static vox_chunk_header* parse_vox_chunk_header(uint8_t** data)
{
    vox_chunk_header* header = (vox_chunk_header*)*data;
    *data += sizeof(vox_chunk_header);
    return header;
}

static vox_str parse_vox_str(uint8_t** data)
{
    vox_str str;
    str.size = **(uint32_t**)data;
    *data += sizeof(uint32_t);
    str.str = *(char**)data;
    *data += str.size;
    return str;
}

static void parse_vox_matl(uint8_t** data, vox_chunk_header* chunk_header, vox* vox)
{
    ASSERT(strncmp(chunk_header->id, "MATL", 4) == 0);
    uint8_t* skip = *data + chunk_header->n_bytes;

    uint32_t mat_id = **(uint32_t**)data;
    *data += sizeof(uint32_t);

    if (mat_id == 256)
    {
        *data = skip;
        return;
    }

    vox_matl matl;
    matl.type = vox_diffuse;

    uint32_t n_keys = **(uint32_t**)data;
    *data += sizeof(uint32_t);

    vox_str type_key = parse_vox_str(data);
    vox_str type_val = parse_vox_str(data);

    if (strncmp(type_key.str, "_type", type_key.size) != 0)
    {
        vox->materials[mat_id] = matl;
        *data = skip;
        return;
    }

    if (strncmp(type_val.str, "_metal", type_val.size) == 0)
    {
        matl.type = vox_metal;
    }
    else if (strncmp(type_val.str, "_emit", type_val.size) == 0)
    {
        matl.type = vox_emissive;
    }
    else if (strncmp(type_val.str, "_glass", type_val.size) == 0)
    {
        matl.type = vox_glass;
    }
    else
    {
        vox->materials[mat_id] = matl;
        *data = skip;
        return;
    }

    char buf[16];
    n_keys--;
    for (uint32_t i = 0; i < n_keys; ++i)
    {
        vox_str key = parse_vox_str(data);
        uint32_t val_size = **(uint32_t**)data;
        ASSERT(val_size < sizeof(buf));
        *data += sizeof(uint32_t);
        memcpy(buf, *data, val_size);
        buf[val_size] = 0;
        *data += val_size;
        float val = atof(buf);
        if (strncmp(key.str, "_rough", key.size) == 0)
        {
            matl.rough = val;
        }
        else if (strncmp(key.str, "_metal", key.size) == 0)
        {
            matl.metal = val;
        }
        else if (strncmp(key.str, "_ior", key.size) == 0)
        {
            matl.ior = val;
        }
        else if (strncmp(key.str, "_sp", key.size) == 0)
        {
            matl.specular = val;
        }
        else if (strncmp(key.str, "_emit", key.size) == 0)
        {
            matl.emit = val;
        }
        else if (strncmp(key.str, "_flux", key.size) == 0)
        {
            matl.flux = val;
        }
        else if (strncmp(key.str, "_trans", key.size) == 0)
        {
            matl.trans = val;
        }
    }

    vox->materials[mat_id] = matl;
}

static void parse_vox_model(uint8_t** data, vox_chunk_header* chunk_header, vox_model* model)
{
    ASSERT(strncmp(chunk_header->id, "SIZE", 4) == 0);
    vox_size* size = (vox_size*)*data;
    *data += sizeof(vox_size);
    model->x = size->x;
    model->y = size->y;
    model->z = size->z;

    chunk_header = parse_vox_chunk_header(data);
    ASSERT(strncmp(chunk_header->id, "XYZI", 4) == 0);
    uint32_t n_voxels = **(uint32_t**)data;
    *data += sizeof(uint32_t);
    vox_xyzi* xyzi = (vox_xyzi*)*data;
    *data += n_voxels * sizeof(vox_xyzi);

    model->voxels = (uint8_t*)malloc(size->x * size->y * size->z);
    memset(model->voxels, 0, size->x * size->y * size->z);
    for (uint32_t i = 0; i < n_voxels; ++i)
    {
        uint32_t index = size->x * size->y * xyzi[i].z + size->x * xyzi[i].y + xyzi[i].x;
        model->voxels[index] = xyzi[i].i;
    }
}

bool vox_load(const char* path, vox* vox)
{
    bul::file file = bul::file::open(path, bul::file::mode::read);
    defer
    {
        file.close();
    };

    bul::scope_allocator allocator = bul::scope_allocator::create_global();
    uint32_t file_size = file.size();
    uint8_t* data = (uint8_t*)allocator.alloc(file_size);
    void* end_of_data = data + file_size;
    file.read(data, file_size);

    vox_header* vox_header = parse_vox_header(&data);
    if (strncmp(vox_header->magic, "VOX ", 4) != 0)
    {
        return false;
    }

    vox_chunk_header* chunk = parse_vox_chunk_header(&data);
    if (strncmp(chunk->id, "MAIN", 4) != 0)
    {
        return false;
    }

    vox_model* current_model;

    chunk = parse_vox_chunk_header(&data);
    if (strncmp(chunk->id, "PACK", 4) == 0)
    {
        vox->n_models = *(uint32_t*)data;
        data += sizeof(uint32_t);
        current_model = vox->models;
        chunk = parse_vox_chunk_header(&data);
    }
    else
    {
        vox->n_models = 1;
        current_model = &vox->model;
    }

    memset(vox->materials, 0, 256 * sizeof(vox_matl));

    bool has_palette = false;
    for (; (uint8_t*)chunk < end_of_data; chunk = parse_vox_chunk_header(&data))
    {
        if (strncmp(chunk->id, "SIZE", 4) == 0)
        {
            parse_vox_model(&data, chunk, current_model);
            current_model++;
        }
        else if (strncmp(chunk->id, "RGBA", 4) == 0)
        {
            has_palette = true;
            memcpy(vox->palette + 1, data, 255 * sizeof(vox_rgba));
            data += 256 * sizeof(vox_rgba);
        }
        else if (strncmp(chunk->id, "MATL", 4) == 0)
        {
            parse_vox_matl(&data, chunk, vox);
        }
        else
        {
            data += chunk->n_bytes;
        }
    }

    if (!has_palette)
    {
        memcpy(vox->palette, default_palette, sizeof(default_palette));
    }

    return true;
}

void vox_unload(vox* vox)
{
    if (vox->n_models == 1)
    {
        free(vox->model.voxels);
    }
    else if (vox->n_models > 1)
    {
        for (uint32_t i = 0; i < vox->n_models; ++i)
        {
            free(vox->models[i].voxels);
        }
        free(vox->models);
    }
    vox->n_models = 0;
}

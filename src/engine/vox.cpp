#include "vox.h"

#include <string.h>
#include <stdlib.h>

#include "bul/bul.h"
#include "bul/file.h"
#include "bul/allocators/scope_allocator.h"

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
    bul::file file = bul::file::open(path, bul::file_mode::read);
    defer
    {
        file.close();
    };

    bul::scope_allocator allocator = bul::scope_allocator::create_global();
    uint32_t file_size = file.size();
    uint8_t* data = (uint8_t*)allocator.alloc(file_size);
    file.read(data, file_size);
    void* end_of_data = data + file_size;

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
        vox->models = (vox_model*)malloc(vox->n_models * sizeof(*vox->models));
        current_model = vox->models;
        chunk = parse_vox_chunk_header(&data);
    }
    else
    {
        vox->n_models = 1;
        current_model = &vox->model;
    }

    memset(vox->materials, 0, 256 * sizeof(vox_matl));

    for (; (uint8_t*)chunk < end_of_data; chunk = parse_vox_chunk_header(&data))
    {
        if (strncmp(chunk->id, "SIZE", 4) == 0)
        {
            parse_vox_model(&data, chunk, current_model);
            current_model++;
        }
        else if (strncmp(chunk->id, "RGBA", 4) == 0)
        {
            vox_rgba* palette = (vox_rgba*)data;
            for (uint32_t i = 1; i < 256; ++i)
            {
                vox->materials[i].color = palette[i];
            }
            data += 256 * sizeof(vox_rgba);
        }
        else if (strncmp(chunk->id, "MATL", 4) == 0)
        {
            parse_vox_matl(&data, chunk, vox);
        }
        else // skip
        {
            data += chunk->n_bytes;
        }
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

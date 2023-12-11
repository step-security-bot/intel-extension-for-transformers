//  Copyright (c) 2023 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
// Defines fileno on msys:

#ifndef GGUF_H
#define GGUF_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#include <cstddef>
#include <cstdint>
#include <cstdio>
#endif

#include "core/layers/jblas_common.hpp"
#include "core/ne_layers.h"
#include "models/model_utils/util.h"

#define GGML_MAX_DIMS           4
#define GGUF_MAGIC "GGUF"

struct gguf_str {
  uint64_t n;  // GGUFv2
  char* data;
};

enum llama_fver {
    GGUF_FILE_VERSION_V1 = 1,
    GGUF_FILE_VERSION_V2 = 2,
    GGUF_FILE_VERSION_V3 = 3,
};

enum ggml_type {
    GGML_TYPE_F32  = 0,
    GGML_TYPE_F16  = 1,
    GGML_TYPE_Q4_0 = 2,
    GGML_TYPE_Q4_1 = 3,
    // GGML_TYPE_Q4_2 = 4, support has been removed
    // GGML_TYPE_Q4_3 (5) support has been removed
    GGML_TYPE_Q5_0 = 6,
    GGML_TYPE_Q5_1 = 7,
    GGML_TYPE_Q8_0 = 8,
    GGML_TYPE_Q8_1 = 9,
    // k-quantizations
    GGML_TYPE_Q2_K = 10,
    GGML_TYPE_Q3_K = 11,
    GGML_TYPE_Q4_K = 12,
    GGML_TYPE_Q5_K = 13,
    GGML_TYPE_Q6_K = 14,
    GGML_TYPE_Q8_K = 15,
    GGML_TYPE_I8,
    GGML_TYPE_I16,
    GGML_TYPE_I32,
    GGML_TYPE_COUNT,
};

enum gguf_type {
  GGUF_TYPE_UINT8 = 0,
  GGUF_TYPE_INT8 = 1,
  GGUF_TYPE_UINT16 = 2,
  GGUF_TYPE_INT16 = 3,
  GGUF_TYPE_UINT32 = 4,
  GGUF_TYPE_INT32 = 5,
  GGUF_TYPE_FLOAT32 = 6,
  GGUF_TYPE_BOOL = 7,
  GGUF_TYPE_STRING = 8,
  GGUF_TYPE_ARRAY = 9,
  GGUF_TYPE_UINT64 = 10,
  GGUF_TYPE_INT64 = 11,
  GGUF_TYPE_FLOAT64 = 12,
  GGUF_TYPE_COUNT,  // marks the end of the enum
};

static const char * GGUF_TYPE_NAME[GGUF_TYPE_COUNT] = {
    [GGUF_TYPE_UINT8]   = "u8",
    [GGUF_TYPE_INT8]    = "i8",
    [GGUF_TYPE_UINT16]  = "u16",
    [GGUF_TYPE_INT16]   = "i16",
    [GGUF_TYPE_UINT32]  = "u32",
    [GGUF_TYPE_INT32]   = "i32",
    [GGUF_TYPE_FLOAT32] = "f32",
    [GGUF_TYPE_BOOL]    = "bool",
    [GGUF_TYPE_STRING]  = "str",
    [GGUF_TYPE_ARRAY]   = "arr",
    [GGUF_TYPE_UINT64]  = "u64",
    [GGUF_TYPE_INT64]   = "i64",
    [GGUF_TYPE_FLOAT64] = "f64",
};

union gguf_value {
  uint8_t uint8;
  int8_t int8;
  uint16_t uint16;
  int16_t int16;
  uint32_t uint32;
  int32_t int32;
  float float32;
  uint64_t uint64;
  int64_t int64;
  double float64;
  bool bool_;

  struct gguf_str str;

  struct {
    enum gguf_type type;

    uint64_t n;  // GGUFv2
    void* data;
  } arr;
};

struct gguf_kv {
  struct gguf_str key;

  enum gguf_type type;
  union gguf_value value;
};

struct gguf_header {
  char magic[4];
  uint32_t version;
  uint64_t n_tensors;  // GGUFv2
  uint64_t n_kv;       // GGUFv2
};

struct gguf_context {
  struct gguf_header header;

  struct gguf_kv* kv;
  struct gguf_tensor_info* infos;

  size_t alignment;
  size_t offset;  // offset of `data` from beginning of file
  size_t size;    // size of `data` in bytes

  // uint8_t * padding;
  void* data;
};

#if UINTPTR_MAX == 0xFFFFFFFF
#define GGML_MEM_ALIGN 4
#else
#define GGML_MEM_ALIGN 16
#endif

#define GGUF_DEFAULT_ALIGNMENT 32

static const size_t GGUF_TYPE_SIZE[GGUF_TYPE_COUNT] = {
    [GGUF_TYPE_UINT8]   = sizeof(uint8_t),
    [GGUF_TYPE_INT8]    = sizeof(int8_t),
    [GGUF_TYPE_UINT16]  = sizeof(uint16_t),
    [GGUF_TYPE_INT16]   = sizeof(int16_t),
    [GGUF_TYPE_UINT32]  = sizeof(uint32_t),
    [GGUF_TYPE_INT32]   = sizeof(int32_t),
    [GGUF_TYPE_FLOAT32] = sizeof(float),
    [GGUF_TYPE_BOOL]    = sizeof(bool),
    [GGUF_TYPE_STRING]  = sizeof(struct gguf_str),
    [GGUF_TYPE_ARRAY]   = 0, // undefined
    [GGUF_TYPE_UINT64]  = sizeof(uint64_t),
    [GGUF_TYPE_INT64]   = sizeof(int64_t),
    [GGUF_TYPE_FLOAT64] = sizeof(double),
};
static_assert(GGUF_TYPE_COUNT == 13, "GGUF_TYPE_COUNT != 13");

struct gguf_tensor_info {
    struct gguf_str name;

    uint32_t n_dims;
    uint64_t ne[GGML_MAX_DIMS];

    enum ggml_type type;

    uint64_t offset; // offset from start of `data`, must be a multiple of `ALIGNMENT`

    // for writing API
    const void * data;
    size_t size;
};

static bool gguf_fread_el(FILE* file, void* dst, size_t size, size_t* offset) {
  const size_t n = fread(dst, 1, size, file);
  *offset += n;
  return n == size;
}

static bool gguf_fread_str(FILE* file, struct gguf_str* p, size_t* offset) {
  p->n = 0;
  p->data = NULL;

  bool ok = true;

  ok = ok && gguf_fread_el(file, &p->n, sizeof(p->n), offset);
  p->data = reinterpret_cast<char*>(calloc(p->n + 1, 1));
  ok = ok && gguf_fread_el(file, p->data, p->n, offset);

  return ok;
}

static const char * llama_file_version_name(llama_fver version) {
    switch (version) {
        case GGUF_FILE_VERSION_V1: return "GGUF V1 (support until nov 2023)";
        case GGUF_FILE_VERSION_V2: return "GGUF V2";
        case GGUF_FILE_VERSION_V3: return "GGUF V3 (latest)";
    }

    return "unknown";
}

inline static void* ggml_aligned_malloc(size_t size) {
  if (size == 0) {
    printf("WARNING: Behavior may be unexpected when allocating 0 bytes for ggml_aligned_malloc!\n");
    return NULL;
  }
  void* aligned_memory = NULL;
#ifdef GGML_USE_CPU_HBM
  int result = hbw_posix_memalign(&aligned_memory, 16, size);
#elif GGML_USE_METAL
  int result = posix_memalign(&aligned_memory, sysconf(_SC_PAGESIZE), size);
#else
  int result = posix_memalign(&aligned_memory, GGML_MEM_ALIGN, size);
#endif
  if (result != 0) {
    // Handle allocation failure
    const char* error_desc = "unknown allocation error";
    switch (result) {
      case EINVAL:
        error_desc = "invalid alignment value";
        break;
      case ENOMEM:
        error_desc = "insufficient memory";
        break;
    }
    printf("%s: %s (attempted to allocate %6.2f MB)\n", __func__, error_desc, size / (1024.0 * 1024.0));
    return NULL;
  }
  return aligned_memory;
}
#define GGML_ALIGNED_MALLOC(size) ggml_aligned_malloc(size)

#define GGUF_GET_KEY(ctx, dst, func, type, req, key)                                                        \
  do {                                                                                                      \
    const std::string skey(key);                                                                            \
    const int kid = gguf_find_key(ctx, skey.c_str());                                                       \
    if (kid >= 0) {                                                                                         \
      enum gguf_type ktype = gguf_get_kv_type(ctx, kid);                                                    \
      if (ktype != (type)) {                                                                                \
        throw std::runtime_error(format("key %s has wrong type: %s", skey.c_str(), gguf_type_name(ktype))); \
      }                                                                                                     \
      (dst) = func(ctx, kid);                                                                               \
    } else if (req) {                                                                                       \
      throw std::runtime_error(format("key not found in model: %s", skey.c_str()));                         \
    }                                                                                                       \
  } while (0)

static void replace_all(std::string & s, const std::string & search, const std::string & replace) {
    std::string result;
    for (size_t pos = 0; ; pos += search.length()) {
        auto new_pos = s.find(search, pos);
        if (new_pos == std::string::npos) {
            result += s.substr(pos, s.size() - pos);
            break;
        }
        result += s.substr(pos, new_pos - pos) + replace;
        pos = new_pos;
    }
    s = std::move(result);
}

enum llm_kv {
    LLM_KV_GENERAL_ARCHITECTURE,
    LLM_KV_GENERAL_QUANTIZATION_VERSION,
    LLM_KV_GENERAL_ALIGNMENT,
    LLM_KV_GENERAL_NAME,
    LLM_KV_GENERAL_AUTHOR,
    LLM_KV_GENERAL_URL,
    LLM_KV_GENERAL_DESCRIPTION,
    LLM_KV_GENERAL_LICENSE,
    LLM_KV_GENERAL_SOURCE_URL,
    LLM_KV_GENERAL_SOURCE_HF_REPO,

    LLM_KV_CONTEXT_LENGTH,
    LLM_KV_EMBEDDING_LENGTH,
    LLM_KV_BLOCK_COUNT,
    LLM_KV_FEED_FORWARD_LENGTH,
    LLM_KV_USE_PARALLEL_RESIDUAL,
    LLM_KV_TENSOR_DATA_LAYOUT,

    LLM_KV_ATTENTION_HEAD_COUNT,
    LLM_KV_ATTENTION_HEAD_COUNT_KV,
    LLM_KV_ATTENTION_MAX_ALIBI_BIAS,
    LLM_KV_ATTENTION_CLAMP_KQV,
    LLM_KV_ATTENTION_LAYERNORM_EPS,
    LLM_KV_ATTENTION_LAYERNORM_RMS_EPS,

    LLM_KV_ROPE_DIMENSION_COUNT,
    LLM_KV_ROPE_FREQ_BASE,
    LLM_KV_ROPE_SCALE_LINEAR,
    LLM_KV_ROPE_SCALING_TYPE,
    LLM_KV_ROPE_SCALING_FACTOR,
    LLM_KV_ROPE_SCALING_ORIG_CTX_LEN,
    LLM_KV_ROPE_SCALING_FINETUNED,

    LLM_KV_TOKENIZER_MODEL,
    LLM_KV_TOKENIZER_LIST,
    LLM_KV_TOKENIZER_TOKEN_TYPE,
    LLM_KV_TOKENIZER_SCORES,
    LLM_KV_TOKENIZER_MERGES,
    LLM_KV_TOKENIZER_BOS_ID,
    LLM_KV_TOKENIZER_EOS_ID,
    LLM_KV_TOKENIZER_UNK_ID,
    LLM_KV_TOKENIZER_SEP_ID,
    LLM_KV_TOKENIZER_PAD_ID,
    LLM_KV_TOKENIZER_ADD_BOS,
    LLM_KV_TOKENIZER_ADD_EOS,
    LLM_KV_TOKENIZER_HF_JSON,
    LLM_KV_TOKENIZER_RWKV,
};

static std::map<llm_kv, std::string> LLM_KV_NAMES = {
    { LLM_KV_GENERAL_ARCHITECTURE,          "general.architecture"                  },
    { LLM_KV_GENERAL_QUANTIZATION_VERSION,  "general.quantization_version"          },
    { LLM_KV_GENERAL_ALIGNMENT,             "general.alignment"                     },
    { LLM_KV_GENERAL_NAME,                  "general.name"                          },
    { LLM_KV_GENERAL_AUTHOR,                "general.author"                        },
    { LLM_KV_GENERAL_URL,                   "general.url"                           },
    { LLM_KV_GENERAL_DESCRIPTION,           "general.description"                   },
    { LLM_KV_GENERAL_LICENSE,               "general.license"                       },
    { LLM_KV_GENERAL_SOURCE_URL,            "general.source.url"                    },
    { LLM_KV_GENERAL_SOURCE_HF_REPO,        "general.source.huggingface.repository" },

    { LLM_KV_CONTEXT_LENGTH,                "%s.context_length"        },
    { LLM_KV_EMBEDDING_LENGTH,              "%s.embedding_length"      },
    { LLM_KV_BLOCK_COUNT,                   "%s.block_count"           },
    { LLM_KV_FEED_FORWARD_LENGTH,           "%s.feed_forward_length"   },
    { LLM_KV_USE_PARALLEL_RESIDUAL,         "%s.use_parallel_residual" },
    { LLM_KV_TENSOR_DATA_LAYOUT,            "%s.tensor_data_layout"    },

    { LLM_KV_ATTENTION_HEAD_COUNT,          "%s.attention.head_count"             },
    { LLM_KV_ATTENTION_HEAD_COUNT_KV,       "%s.attention.head_count_kv"          },
    { LLM_KV_ATTENTION_MAX_ALIBI_BIAS,      "%s.attention.max_alibi_bias"         },
    { LLM_KV_ATTENTION_CLAMP_KQV,           "%s.attention.clamp_kqv"              },
    { LLM_KV_ATTENTION_LAYERNORM_EPS,       "%s.attention.layer_norm_epsilon"     },
    { LLM_KV_ATTENTION_LAYERNORM_RMS_EPS,   "%s.attention.layer_norm_rms_epsilon" },

    { LLM_KV_ROPE_DIMENSION_COUNT,          "%s.rope.dimension_count"                 },
    { LLM_KV_ROPE_FREQ_BASE,                "%s.rope.freq_base"                       },
    { LLM_KV_ROPE_SCALE_LINEAR,             "%s.rope.scale_linear"                    },
    { LLM_KV_ROPE_SCALING_TYPE,             "%s.rope.scaling.type"                    },
    { LLM_KV_ROPE_SCALING_FACTOR,           "%s.rope.scaling.factor"                  },
    { LLM_KV_ROPE_SCALING_ORIG_CTX_LEN,     "%s.rope.scaling.original_context_length" },
    { LLM_KV_ROPE_SCALING_FINETUNED,        "%s.rope.scaling.finetuned"               },

    { LLM_KV_TOKENIZER_MODEL,               "tokenizer.ggml.model"              },
    { LLM_KV_TOKENIZER_LIST,                "tokenizer.ggml.tokens"             },
    { LLM_KV_TOKENIZER_TOKEN_TYPE,          "tokenizer.ggml.token_type"         },
    { LLM_KV_TOKENIZER_SCORES,              "tokenizer.ggml.scores"             },
    { LLM_KV_TOKENIZER_MERGES,              "tokenizer.ggml.merges"             },
    { LLM_KV_TOKENIZER_BOS_ID,              "tokenizer.ggml.bos_token_id"       },
    { LLM_KV_TOKENIZER_EOS_ID,              "tokenizer.ggml.eos_token_id"       },
    { LLM_KV_TOKENIZER_UNK_ID,              "tokenizer.ggml.unknown_token_id"   },
    { LLM_KV_TOKENIZER_SEP_ID,              "tokenizer.ggml.seperator_token_id" },
    { LLM_KV_TOKENIZER_PAD_ID,              "tokenizer.ggml.padding_token_id"   },
    { LLM_KV_TOKENIZER_ADD_BOS,             "tokenizer.ggml.add_bos_token"      },
    { LLM_KV_TOKENIZER_ADD_EOS,             "tokenizer.ggml.add_eos_token"      },
    { LLM_KV_TOKENIZER_HF_JSON,             "tokenizer.huggingface.json"        },
    { LLM_KV_TOKENIZER_RWKV,                "tokenizer.rwkv.world"              },
};

static std::string gguf_data_to_str(enum gguf_type type, const void * data, int i) {
    switch (type) {
        case GGUF_TYPE_UINT8:   return std::to_string(((const uint8_t  *)data)[i]);
        case GGUF_TYPE_INT8:    return std::to_string(((const int8_t   *)data)[i]);
        case GGUF_TYPE_UINT16:  return std::to_string(((const uint16_t *)data)[i]);
        case GGUF_TYPE_INT16:   return std::to_string(((const int16_t  *)data)[i]);
        case GGUF_TYPE_UINT32:  return std::to_string(((const uint32_t *)data)[i]);
        case GGUF_TYPE_INT32:   return std::to_string(((const int32_t  *)data)[i]);
        case GGUF_TYPE_UINT64:  return std::to_string(((const uint64_t *)data)[i]);
        case GGUF_TYPE_INT64:   return std::to_string(((const int64_t  *)data)[i]);
        case GGUF_TYPE_FLOAT32: return std::to_string(((const float    *)data)[i]);
        case GGUF_TYPE_FLOAT64: return std::to_string(((const double   *)data)[i]);
        case GGUF_TYPE_BOOL:    return ((const bool *)data)[i] ? "true" : "false";
        default:                return format("unknown type %d", type);
    }
}


#endif  // GGUF_H
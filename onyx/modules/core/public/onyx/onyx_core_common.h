#pragma once

#include <algorithm>
#include <atomic>
#include <bit>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <functional>
#include <limits>
#include <memory>
#include <numbers>
#include <queue>
#include <ranges>
#include <set>
#include <stack>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 512
#include <magic_enum.hpp>

#include <onyx/assert.h>
#include <onyx/onyx_types.h>
#include <onyx/bitwiseenum.h>
#include <onyx/guid.h>
#include <onyx/hash.h>
#include <onyx/math.h>
#include <onyx/reference.h>
#include <onyx/time.h>
#include <onyx/inplacefunction.h>
#include <onyx/log/logger.h>
#include <onyx/container/inplacearray.h>
#include <onyx/container/typelist.h>
#include <onyx/function/callback.h>
#include <onyx/function/signal.h>
#include <onyx/geometry/common.h>
#include <onyx/string/inplacestring.h>
#include <onyx/string/string.h>
#include <onyx/stringid.h>
#include <onyx/typetraits.h>
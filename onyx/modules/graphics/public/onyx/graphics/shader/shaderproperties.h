#pragma once
#include <onyx/filesystem/path.h>

namespace Onyx::Graphics
{
    struct ShaderProperties
    {
        FileSystem::Filepath Path;
        DynamicArray<String> AdditionalIncludes;
    };
}

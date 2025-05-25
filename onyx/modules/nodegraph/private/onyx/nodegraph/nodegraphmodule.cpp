#include <onyx/nodegraph/nodegraphmodule.h>

#include <onyx/nodegraph/nodegraphfactory.h>

#include <onyx/nodegraph/nodes/math/arithmeticnodes.h>
#include <onyx/nodegraph/nodes/math/geometricnodes.h>
#include <onyx/nodegraph/nodes/math/vectornodes.h>

namespace Onyx::NodeGraph
{
    void Init()
    {
        NodeGraphTypeRegistry::RegisterType<bool, "bool">();
        NodeGraphTypeRegistry::RegisterType<onyxF32, "onyxF32">();
        NodeGraphTypeRegistry::RegisterType<onyxF64, "onyxF64">();
        NodeGraphTypeRegistry::RegisterType<onyxS32, "onyxS32">();
        NodeGraphTypeRegistry::RegisterType<onyxS64, "onyxS64">();
        NodeGraphTypeRegistry::RegisterType<String, "string">();
        NodeGraphTypeRegistry::RegisterType<Vector2s32, "vector2s32">();
        NodeGraphTypeRegistry::RegisterType<Vector2s64, "vector2s64">();
        NodeGraphTypeRegistry::RegisterType<Vector2f, "vector2f32">();
        NodeGraphTypeRegistry::RegisterType<Vector2d, "vector2f64">();
        NodeGraphTypeRegistry::RegisterType<Vector3s32, "vector3s32">();
        NodeGraphTypeRegistry::RegisterType<Vector3s64, "vector3s64">();
        NodeGraphTypeRegistry::RegisterType<Vector3f, "vector3f32">();
        NodeGraphTypeRegistry::RegisterType<Vector3d, "vector3f64">();
        NodeGraphTypeRegistry::RegisterType<Vector4s32, "vector4s32">();
        NodeGraphTypeRegistry::RegisterType<Vector4s64, "vector4s64">();
        NodeGraphTypeRegistry::RegisterType<Vector4f, "vector4f32">();
        NodeGraphTypeRegistry::RegisterType<Vector4d, "vector4f64">();
        NodeGraphTypeRegistry::RegisterType<ExecutePin, "execute">();

        RegisterArithmeticNodes<NodeGraphFactory, "NodeGraph">();
        RegisterGeometricNodes<NodeGraphFactory, "NodeGraph">();
        RegisterVectorNodes<NodeGraphFactory, "NodeGraph">();
    }
}

#include <onyx/nodegraph/nodegraphmodule.h>

#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/nodegraphtyperegistry.h>

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
        NodeGraphTypeRegistry::RegisterType<Vector2f32, "vector2f32">();
        NodeGraphTypeRegistry::RegisterType<Vector2f64, "vector2f64">();
        NodeGraphTypeRegistry::RegisterType<Vector3s32, "vector3s32">();
        NodeGraphTypeRegistry::RegisterType<Vector3s64, "vector3s64">();
        NodeGraphTypeRegistry::RegisterType<Vector3f32, "vector3f32">();
        NodeGraphTypeRegistry::RegisterType<Vector3f64, "vector3f64">();
        NodeGraphTypeRegistry::RegisterType<Vector4s32, "vector4s32">();
        NodeGraphTypeRegistry::RegisterType<Vector4s64, "vector4s64">();
        NodeGraphTypeRegistry::RegisterType<Vector4f32, "vector4f32">();
        NodeGraphTypeRegistry::RegisterType<Vector4f64, "vector4f64">();
        NodeGraphTypeRegistry::RegisterType<ExecutePin, "execute">();

        RegisterArithmeticNodes<NodeGraphFactory, "Onyx::NodeGraph">();
        RegisterGeometricNodes<NodeGraphFactory, "Onyx::NodeGraph">();
        RegisterVectorNodes<NodeGraphFactory, "Onyx::NodeGraph">();
    }
}

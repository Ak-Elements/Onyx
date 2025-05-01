#include <onyx/nodegraph/nodegraphmodule.h>

#include <onyx/nodegraph/nodegraphfactory.h>

#include <onyx/nodegraph/nodes/math/arithmeticnodes.h>
#include <onyx/nodegraph/nodes/math/geometricnodes.h>
#include <onyx/nodegraph/nodes/math/vectornodes.h>

namespace Onyx::NodeGraph
{
    void Init()
    {
        RegisterArithmeticNodes<NodeGraphFactory, "NodeGraph">();
        RegisterGeometricNodes<NodeGraphFactory, "NodeGraph">();
        RegisterVectorNodes<NodeGraphFactory, "NodeGraph">();
    }
}

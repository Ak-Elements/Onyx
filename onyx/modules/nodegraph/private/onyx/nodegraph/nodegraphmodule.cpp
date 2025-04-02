#include <onyx/nodegraph/nodefactory.h>
#include <onyx/nodegraph/nodegraphmodule.h>

#include <onyx/nodegraph/nodes/math/arithmeticnodes.h>
#include <onyx/nodegraph/nodes/math/geometricnodes.h>
#include <onyx/nodegraph/nodes/math/vectornodes.h>

namespace Onyx::NodeGraph
{
    void Init()
    {
        RegisterArithmeticNodes<NodeFactory>();
        RegisterGeometricNodes<NodeFactory>();
        RegisterVectorNodes<NodeFactory>();
    }
}

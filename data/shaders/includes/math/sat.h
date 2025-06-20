vec2 GetTriangleOnEdgeProjection(vec2 triangleP0, vec2 triangleP1, vec2 triangleP2, vec2 edgeNormal)
{
    float minValue = dot(triangleP0, edgeNormal);
    float maxValue = minValue;

    float projected = dot(triangleP1, edgeNormal);
    minValue = min(minValue, projected);
    maxValue = max(maxValue, projected);

    projected = dot(triangleP2, edgeNormal);
    minValue = min(minValue, projected);
    maxValue = max(maxValue, projected);
    return vec2(minValue, maxValue);
}

vec2 GetLineOnEdgeProjection(vec2 lineP0, vec2 lineP1, vec2 edgeNormal)
{
    float minValue = dot(lineP0, edgeNormal);
    float maxValue = minValue;

    float projected = dot(lineP1, edgeNormal);
    minValue = min(minValue, projected);
    maxValue = max(maxValue, projected);

    return vec2(minValue, maxValue);
}

bool SAT_2D_IsOverlapping(vec2 triangleP0, vec2 triangleP1, vec2 triangleP2, vec2 lineP0, vec2 lineP1)
{
    // edge 0
    vec2 edgeNormal = triangleP1 - triangleP0;
    edgeNormal = vec2(edgeNormal.y, -edgeNormal.x);

    vec2 projected0 = GetTriangleOnEdgeProjection(triangleP0, triangleP1, triangleP2, edgeNormal);
    vec2 projected1 = GetLineOnEdgeProjection(lineP0, lineP1, edgeNormal);
    if ((projected0.y < projected1.x) ||
        (projected1.y < projected0.x))
    {
        return false;
    }

    // edge 1
    edgeNormal = triangleP2 - triangleP1;
    edgeNormal = vec2(edgeNormal.y, -edgeNormal.x);

    projected0 = GetTriangleOnEdgeProjection(triangleP0, triangleP1, triangleP2, edgeNormal);
    projected1 = GetLineOnEdgeProjection(lineP0, lineP1, edgeNormal);
    if ((projected0.y < projected1.x) ||
        (projected1.y < projected0.x))
    {
        return false;
    }

    // edge 2
    edgeNormal = triangleP0 - triangleP2;
    edgeNormal = vec2(edgeNormal.y, -edgeNormal.x);

    projected0 = GetTriangleOnEdgeProjection(triangleP0, triangleP1, triangleP2, edgeNormal);
    projected1 = GetLineOnEdgeProjection(lineP0, lineP1, edgeNormal);
    if ((projected0.y < projected1.x) ||
        (projected1.y < projected0.x))
    {
        return false;
    }

    // test line normal
    vec2 lineNormal = lineP1 - lineP0;
    lineNormal = vec2(lineNormal.y, -lineNormal.x);

    projected0 = GetTriangleOnEdgeProjection(triangleP0, triangleP1, triangleP2, lineNormal);
    projected1 = GetLineOnEdgeProjection(lineP0, lineP1, lineNormal);
    if ((projected0.y < projected1.x) ||
        (projected1.y < projected0.x))
    {
        return false;
    }

    return true;
}

bool SAT_2D_IsOverlapping(vec2 triangleP0, vec2 triangleP1, vec2 triangleP2, vec2 triangle1P0, vec2 triangle1P1, vec2 triangle1P2)
{   
    // triangle 0 edge 0
    vec2 edgeNormal = triangleP1 - triangleP0;
    edgeNormal = vec2(edgeNormal.y, -edgeNormal.x);

    vec2 projected0 = GetTriangleOnEdgeProjection(triangleP0, triangleP1, triangleP2, edgeNormal);
    vec2 projected1 = GetTriangleOnEdgeProjection(triangle1P0, triangle1P1, triangle1P2, edgeNormal);
    if ((projected0.y < projected1.x) ||
        (projected1.y < projected0.x))
    {
        return false;
    }

    // triangle 0 edge 1
    edgeNormal = triangleP2 - triangleP1;
    edgeNormal = vec2(edgeNormal.y, -edgeNormal.x);

    projected0 = GetTriangleOnEdgeProjection(triangleP0, triangleP1, triangleP2, edgeNormal);
    projected1 = GetTriangleOnEdgeProjection(triangle1P0, triangle1P1, triangle1P2, edgeNormal);
    if ((projected0.y < projected1.x) ||
        (projected1.y < projected0.x))
    {
        return false;
    }

    // triangle 0 edge 2
    edgeNormal = triangleP0 - triangleP2;
    edgeNormal = vec2(edgeNormal.y, -edgeNormal.x);

    projected0 = GetTriangleOnEdgeProjection(triangleP0, triangleP1, triangleP2, edgeNormal);
    projected1 = GetTriangleOnEdgeProjection(triangle1P0, triangle1P1, triangle1P2, edgeNormal);
    if ((projected0.y < projected1.x) ||
        (projected1.y < projected0.x))
    {
        return false;
    }

    // triangle 1 edge 0
    edgeNormal = triangle1P1 - triangle1P0;
    edgeNormal = vec2(edgeNormal.y, -edgeNormal.x);

    projected0 = GetTriangleOnEdgeProjection(triangleP0, triangleP1, triangleP2, edgeNormal);
    projected1 = GetTriangleOnEdgeProjection(triangle1P0, triangle1P1, triangle1P2, edgeNormal);
    if ((projected0.y < projected1.x) ||
        (projected1.y < projected0.x))
    {
        return false;
    }

    // triangle 1 edge 1
    edgeNormal = triangle1P2 - triangle1P1;
    edgeNormal = vec2(edgeNormal.y, -edgeNormal.x);

    projected0 = GetTriangleOnEdgeProjection(triangleP0, triangleP1, triangleP2, edgeNormal);
    projected1 = GetTriangleOnEdgeProjection(triangle1P0, triangle1P1, triangle1P2, edgeNormal);
    if ((projected0.y < projected1.x) ||
        (projected1.y < projected0.x))
    {
        return false;
    }

    // triangle 1 edge 2
    edgeNormal = triangle1P0 - triangle1P2;
    edgeNormal = vec2(edgeNormal.y, -edgeNormal.x);

    projected0 = GetTriangleOnEdgeProjection(triangleP0, triangleP1, triangleP2, edgeNormal);
    projected1 = GetTriangleOnEdgeProjection(triangle1P0, triangle1P1, triangle1P2, edgeNormal);
    if ((projected0.y < projected1.x) ||
        (projected1.y < projected0.x))
    {
        return false;
    }

    return true;
}
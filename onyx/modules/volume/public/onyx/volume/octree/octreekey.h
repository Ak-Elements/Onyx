#pragma once

namespace Onyx::Volume
{
template<typename KeyType>
class OctreeKey
{
    static_assert(std::is_integral<KeyType>::value, "Key type has to be an integral.");
public:
    typedef OctreeKey<KeyType> OctreeKeyT;

    static constexpr onyxU8 MaxDepth = sizeof(KeyType) * 8;

    OctreeKey()
        : m_Key {0, 0, 0}
    {
    }

    OctreeKey(KeyType x, KeyType y, KeyType z)
        : m_Key { x, y, z}
    {
    }

    ~OctreeKey()
    {
    }

    OctreeKey(const OctreeKeyT& other)
    {
        std::copy(std::begin(other.m_Key), std::end(other.m_Key), std::begin(m_Key));
    }

    OctreeKey(OctreeKey&& other)
        : m_Key { other.m_Key[0], other.m_Key[1], other.m_Key[2] }
    {
    }

    OctreeKey& operator=(const OctreeKey& other)
    {
        if (this == &other)
            return *this;

        std::copy(std::begin(other.m_Key), std::end(other.m_Key), std::begin(m_Key));

        return *this;
    }

    OctreeKey& operator=(OctreeKey&& other) noexcept
    {
        if (this == &other)
            return *this;
        
        m_Key = std::move(other.m_Key);

        return *this;
    }

    bool operator== (const OctreeKeyT& other) const
    {
        return (m_Key[0] == other.m_Key[0]) && (m_Key[1] == other.m_Key[1]) && (m_Key[2] == other.m_Key[2]);
    }

    bool operator != (const OctreeKeyT& other) const
    {
        return operator==(other) == false;
    }

    bool operator <= (const OctreeKey& other) const
    {
        return (other.m_Key[0] >= m_Key[0]) && (other.m_Key[1] >= m_Key[1]) && (other.m_Key[2] >= m_Key[2]);
    }

    bool operator >= (const OctreeKey& other) const
    {
        return (other.m_Key[0] <= m_Key[0]) && (other.m_Key[1] <= m_Key[1]) && (other.m_Key[2] <= m_Key[2]);
    }

    void TraverseDown(KeyType depth, onyxU8 childIndex)
    {
        /*m_Key[0] = m_Key[0] | ((!!(childIndex & (1 << 0))) << depth);
        m_Key[1] = m_Key[1] | ((!!(childIndex & (1 << 1))) << depth);
        m_Key[2] = m_Key[2] | ((!!(childIndex & (1 << 2))) << depth);*/

        if (childIndex == 1)
        {
            m_Key[0] = m_Key[0] | ((1 << 0) << depth);
        }
        
        if (childIndex == 2)
        {
            m_Key[0] = m_Key[0] | ((1 << 0) << depth);
            m_Key[2] = m_Key[2] | ((1 << 0) << depth);
        }

        if (childIndex == 3)
        {
            m_Key[2] = m_Key[2] | ((1 << 0) << depth);
        }

        if (childIndex == 4)
        {
            m_Key[1] = m_Key[1] | ((1 << 0) << depth);
        }

        if (childIndex == 5)
        {
            m_Key[1] = m_Key[1] | ((1 << 0) << depth);
            m_Key[0] = m_Key[0] | ((1 << 0) << depth);
        }

        if (childIndex == 6)
        {
            m_Key[1] = m_Key[1] | ((1 << 0) << depth);
            m_Key[0] = m_Key[0] | ((1 << 0) << depth);
            m_Key[2] = m_Key[2] | ((1 << 0) << depth);
        }

        if (childIndex == 7)
        {
            m_Key[1] = m_Key[1] | ((1 << 0) << depth);
            m_Key[2] = m_Key[2] | ((1 << 0) << depth);
        }
    }

    void TraverseUp(KeyType depth)
    {
        const KeyType flag = ~(1 << depth);
        m_Key[0] &= flag;
        m_Key[1] &= flag;
        m_Key[2] &= flag;
    }

    // returns the child index which has to be traversed down/up to get to the target node of this key
    onyxU8 GetChildBranchBit(KeyType level) const
    {
        /*return static_cast<uint8> (((!!(m_Key[0] & level)) << 0)
            | ((!!(m_Key[1] & level)) << 1)
            | (!!(m_Key[2] & level)) << 2);*/


        const bool isX = !!(m_Key[0] & level);
        const bool isY = !!(m_Key[1] & level);
        const bool isZ = !!(m_Key[2] & level);

        if (!isX && !isY && !isZ)
        {
            return 0;
        }

        if (isX && !isY && !isZ)
        {
            return 1;
        }

        if (isX && !isY && isZ)
        {
            return 2;
        }

        if (!isX && !isY && isZ)
        {
            return 3;
        }

        if (!isX && isY && !isZ)
        {
            return 4;
        }

        if (isX && isY && !isZ)
        {
            return 5;
        }

        if (isX && isY && isZ)
        {
            return 6;
        }

        if (!isX && isY && isZ)
        {
            return 7;
        }

        ONYX_ASSERT(false);
        return -1;

    }

    KeyType operator[](size_t index) const { return m_Key[index]; }

    Vector3f GetNodeRealPosition(const onyxF32 rootSize, KeyType depth) const
    {
        Vector3f position(0.0f, 0.0f, 0.0f);
        while (depth < (MaxDepth - 1))
        {
            onyxU32 depthMask = 1 << depth;
            //uint8 childIndex = GetChildBranchBit(depthMask);
            const bool isX = !!(m_Key[0] & depthMask);
            const bool isY = !!(m_Key[1] & depthMask);
            const bool isZ = !!(m_Key[2] & depthMask);

            onyxF32 cellSize = rootSize / (1 << (MaxDepth - depth));
            if (isX)
                position[0] += cellSize;
            else
                position[0] -= cellSize;

            if (isY)
                position[1] += cellSize;
            else
                position[1] -= cellSize;

            if (isZ)
                position[2] += cellSize;
            else
                position[2] -= cellSize;

            ++depth;
        }

        return position;
    }

private:
    KeyType m_Key[3];
};

}

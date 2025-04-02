#pragma once

namespace Onyx::Volume
{
    template <typename T, typename KeyT, onyxU8 NumChildren>
    inline Tree<T, KeyT, NumChildren>::Tree()
    {
        m_Nodes.reserve(128);
        m_Data.reserve(256);

        m_Nodes.emplace_back(); // emplace root
    }

    template <typename T, typename KeyT, onyxU8 NumChildren>
    inline Tree<T, KeyT, NumChildren>::~Tree()
    {
    }


    template <typename DataT, typename KeyT /*= MortonCode2D_32*/, onyxU8 NumChildren>
    void Onyx::Tree<DataT, KeyT, NumChildren>::SplitNode(TreeNode& node)
    {
        ONYX_ASSERT(node.IsLeaf());

        onyxS32 firstChildIndex = INVALID_INDEX_32;
        if (m_NextIndex == INVALID_INDEX_32)
        {
            firstChildIndex = static_cast<onyxS32>(m_Nodes.size());
            for (onyxS32 index = 0; index < NumChildren; ++index)
            {
                m_Nodes.emplace_back();
            }
        }
        else
        {
            firstChildIndex = m_NextIndex;
            m_NextIndex = m_Nodes[firstChildIndex].GetIndex();
           

            for (onyxS32 index = 0; index < NumChildren; ++index)
            {
#if ONYX_IS_DEBUG
                m_Nodes[firstChildIndex + index].SetIsValidNode(true);
#endif 
                m_Nodes[firstChildIndex + index].MakeLeaf();
            }

        }

        ONYX_ASSERT(firstChildIndex != INVALID_INDEX_32);

        node.MakeBranch(firstChildIndex);
    }

    template<typename DataT, typename KeyT, onyxU8 NumChildren>
    inline void Tree<DataT, KeyT, NumChildren>::PruneNode(TreeNode& node)
    {
        ONYX_ASSERT(node.IsBranch());

        onyxS32 nodeIndex = node.GetIndex();
        m_Nodes[nodeIndex].SetIndex(m_NextIndex);

#if ONYX_IS_DEBUG
        for (onyxS32 index = 0; index < NumChildren; ++index)
        {
            m_Nodes[nodeIndex + index].SetIsValidNode(false);
        }
#endif 

        m_NextIndex = nodeIndex;
        node.MakeLeaf();
    }

    template<typename DataT, typename KeyT, onyxU8 NumChildren>
    inline DataT& Tree<DataT, KeyT, NumChildren>::GetNodeData(const TreeNode& node)
    {
        ONYX_ASSERT(node.IsLeaf());
        ONYX_ASSERT(node.GetIndex() != INVALID_INDEX_32);
        ONYX_ASSERT(node.GetIndex() < m_Data.size());

        return m_Data[node.GetIndex()];

    }

    template <typename DataT, typename KeyT /*= MortonCode2D_32*/, onyxU8 NumChildren /*= 4*/>
    const DataT& Tree<DataT, KeyT, NumChildren>::GetNodeData(const TreeNode& node) const
    {
        ONYX_ASSERT(node.IsLeaf());
        ONYX_ASSERT(node.GetIndex() != INVALID_INDEX_32);
        ONYX_ASSERT(node.GetIndex() < m_Data.size());

        return m_Data[node.GetIndex()];
    }

}
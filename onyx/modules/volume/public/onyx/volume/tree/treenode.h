#pragma once

namespace Onyx::Volume
{
    class TreeNode
    {
    public:
        inline bool IsLeaf() const
        {
#if ONYX_IS_DEBUG
            ONYX_ASSERT(m_IsValidNode, "Invalid node");
#endif 
            return !m_IsBranch;
        }
        inline bool IsBranch() const
        {
#if ONYX_IS_DEBUG
            ONYX_ASSERT(m_IsValidNode, "Invalid node");
#endif 
            return m_IsBranch;
        }

        inline onyxS32 GetIndex() const
        {
            return m_Index;
        }

        inline void SetIndex(onyxS32 index)
        {
#if ONYX_IS_DEBUG
            ONYX_ASSERT(m_IsValidNode, "Invalid node");
#endif 
            m_Index = index;
        }

#if ONYX_IS_DEBUG
        void SetIsValidNode(bool valid)
        {
            m_IsValidNode = valid;
        }
#endif

        inline void MakeBranch(onyxS32 firstChildIndex)
        {
#if ONYX_IS_DEBUG
            ONYX_ASSERT(m_IsValidNode, "Invalid node");
#endif 
            ONYX_ASSERT(IsLeaf(), "Can not call MakeBranch on an already branched node.");
            m_Index = firstChildIndex;
            m_IsBranch = true;
        }

        inline void MakeLeaf()
        {
#if ONYX_IS_DEBUG
            ONYX_ASSERT(m_IsValidNode, "Invalid node");
#endif 
            m_Index = INVALID_INDEX_32;
            m_IsBranch = false;
        }

    private:
        onyxS32 m_Index = INVALID_INDEX_32; // if its a leaf it's the index to the data, for branches this is the index to the first child
        bool m_IsBranch = false;

#if ONYX_IS_DEBUG
        bool m_IsValidNode = true;
#endif
    };
}
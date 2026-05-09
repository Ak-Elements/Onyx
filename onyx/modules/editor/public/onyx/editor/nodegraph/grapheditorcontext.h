#pragma once

#include <onyx/filesystem/path.h>
#include <onyx/nodegraph/nodegraphfactory.h>
#include <onyx/nodegraph/pins/pinbase.h>

#include <utility>

namespace onyx::ui {
struct TreeItem;
}

namespace onyx::localization {
class LocalizationModule;
}

namespace onyx::assets {
class AssetSystem;
struct AssetMetaData;
} // namespace onyx::assets

namespace onyx::editor {
class GraphEditorContext {
  public:
    GraphEditorContext() = default;
    virtual ~GraphEditorContext() = default;

    enum class PinDirection : uint8_t { Invalid, Input, Output };

    struct Pin {
        Guid64 Id;
        Guid64 LinkId;
        uint32_t LocalId;
        String Name;

        PinDirection Direction;
        node_graph::PinTypeId PinTypeId;
        uint32_t Color;
    };

    struct Node {
        Node( Guid64 id )
            : Id( id ) {}

        Node( Guid64 id, String name )
            : Id( id )
            , Name( std::move( name ) ) {}

        Guid64 Id;
        int8_t LocalId = InvalidIndex8;

        String Name;
        Vector2f32 Position;

        DynamicArray< Pin > Inputs;
        DynamicArray< Pin > Outputs;

        bool HasUpdatedPosition = false;
        bool ShowNodeName = true;
    };

    struct Link {
        Guid64 Id;

        Guid64 FromPinId;
        Guid64 ToPinId;

        // stored to simplify deleting of nodes
        Guid64 FromNodeId;
        Guid64 ToNodeId;

        uint32_t Color = 0xFFFFFFFF;

        bool IsRerouted = false;
    };

    void setLocalizationModule( const localization::LocalizationModule& localizationModule );
    ONYX_NO_DISCARD const localization::LocalizationModule& getLocalizationModule() const;

    DynamicArray< Node >& getNodes() { return m_nodes; }
    ONYX_NO_DISCARD const DynamicArray< Node >& getNodes() const { return m_nodes; }
    DynamicArray< Link >& getLinks() { return m_links; }
    ONYX_NO_DISCARD const DynamicArray< Link >& getLinks() const { return m_links; }

    void clear();

    ONYX_NO_DISCARD virtual StringView getLocalizedAssetTypeName() const = 0;
    ONYX_NO_DISCARD virtual DynamicArray< StringView > getExtensions() const = 0;
    ONYX_NO_DISCARD virtual uint32_t getCanvasBackgroundColor() const = 0;

    Node& createNewNode( StringId32 nodeTypeId );
    void deleteNode( Guid64 nodeId );

    Node& getNode( Guid64 nodeId );
    ONYX_NO_DISCARD const Node& getNode( Guid64 nodeId ) const;
    Node& getNodeForPin( Guid64 pinId );
    ONYX_NO_DISCARD const Node& getNodeForPin( Guid64 pinId ) const;

    void setNodeName( Guid64 nodeId, const String& name );
    void setNodePosition( Guid64 nodeId, const Vector2f32& position );

    ONYX_NO_DISCARD virtual bool isNewLinkValid( Guid64 fromPinId, Guid64 toPinId ) const = 0;
    Link& createNewLink( Guid64 fromPinId, Guid64 toPinId );
    void deleteLink( Guid64 linkId );
    void deleteLink( Guid64 fromPinId, Guid64 toPinId );

    Pin& getPin( Guid64 pinId );
    ONYX_NO_DISCARD const Pin& getPin( Guid64 pinId ) const;
    ONYX_NO_DISCARD bool hasPin( Guid64 pinId ) const;
    ONYX_NO_DISCARD bool isPinLinked( Guid64 pindId ) const;
    ONYX_NO_DISCARD virtual bool arePinTypesCompatible( node_graph::PinTypeId lhsPinType,
                                                        node_graph::PinTypeId rhsPinType ) const = 0;

    void drawNode( const Node& node );
    void drawNodeBackground( const Node& node );

    virtual void drawNodeInPropertyPanel( Guid64 nodeId ) = 0;

    virtual void filterNodeListContextMenu(
        InplaceFunction< bool( StringView, const node_graph::NodeEditorMetaData& ) > filterFunctor ) = 0;
    virtual void clearNodeListFilter() = 0;
    virtual const ui::TreeItem& getNodeListContextMenuRoot() = 0;

    virtual bool compile() = 0;

    void load( assets::AssetSystem& assetSystem, const FilePath& path );
    void save( assets::AssetSystem& assetSystem, const assets::AssetMetaData& assetMeta );
    ONYX_NO_DISCARD bool isLoading() const { return m_isLoading; };

    Callback< void() > OnLoaded;
    Callback< void() > OnSaved;

    Callback< void( Node& ) > OnNodeCreated;
    Callback< void( Node& ) > OnNodeDeleted;
    Callback< void( const Link& ) > OnLinkCreated;
    Callback< void( const Link& ) > OnLinkDeleted;

    Callback< void( const FilePath& ) > LoadEditorMetaDataFunctor;
    Callback< void( const FilePath& ) > SaveEditorMetaDataFunctor;

  protected:
    ONYX_NO_DISCARD virtual const node_graph::INodeFactory& getNodeFactory() const = 0;
    void finishLoading( const assets::AssetMetaData& assetMeta );

  private:
    virtual void onSave( assets::AssetSystem& assetSystem, const assets::AssetMetaData& assetMeta ) = 0;
    virtual void onLoad( assets::AssetSystem& assetSystem, const FilePath& path ) = 0;

    virtual void onDrawNode( const Node& node ) = 0;
    virtual void onDrawNodeBackground( const Node& node ) = 0;
    virtual void onNodeChanged( const Node& newNode );

    virtual bool onNodeCreate( Node& newNode, StringId32 typeId ) = 0;
    virtual void onNodeDelete( Node& nodeToDelete ) = 0;
    virtual void onLinkCreate( const Link& newLink ) = 0;
    virtual void onLinkDelete( const Link& linkToDelete ) = 0;

  private:
    DynamicArray< Node > m_nodes;
    DynamicArray< Link > m_links;

    Atomic< bool > m_isLoading = false;
    const localization::LocalizationModule* m_localizationModule = nullptr;
};
} // namespace onyx::editor

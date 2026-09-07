#pragma once

#include <onyx/localization/localization.h>
#include <onyx/ui/scopeddisable.h>
#include <onyx/ui/scopedindent.h>

namespace onyx::ui::property_grid {

enum class CollectionFlags { None = 0, Append = 1 << 0, Insert = 1 << 1, Reorder = 1 << 2, Delete = 1 << 3 };

struct CollectionOptions {
    CollectionFlags Flags = CollectionFlags::Append | CollectionFlags::Insert | CollectionFlags::Reorder |
                            CollectionFlags::Delete;
};

namespace details {

enum class CollectionEditType : uint8_t {
    None,
    Delete,
    Insert,
    Append,
    Move,
};

template < typename T >
struct CollectionEdit {
    CollectionEditType Type = CollectionEditType::None;

    uint32_t Index = 0;
    uint32_t NewIndex = 0;

    T Value{};
};

template < typename T >
inline void applyEdit( const CollectionEdit< T >& editAction, DynamicArray< T >& collection ) {
    switch( editAction.Type ) {
        using enum CollectionEditType;
    case None:
        break;
    case Delete:
        collection.erase( collection.begin() + editAction.Index );
        break;
    case Insert:
        collection.emplace( collection.begin() + editAction.Index );
        break;
    case Append:
        collection.emplace_back( editAction.Value );
        break;
    case Move: {
        auto begin = collection.begin();
        if( editAction.Index < editAction.NewIndex ) {
            std::rotate( begin + editAction.Index, begin + editAction.Index + 1, begin + editAction.NewIndex + 1 );
        } else if( editAction.Index > editAction.NewIndex ) {
            std::rotate( begin + editAction.NewIndex, begin + editAction.Index, begin + editAction.Index + 1 );
        }
        break;
    }
    }
}

template < typename T >
inline InplaceString< 32 > makeReorderPayloadType( const T& item ) {
    InplaceString< 32 > id;
    format::formatTo2( id, "reorderDrag_{}", (void*)&item );
    return id;
}

template < typename T >
inline Optional< CollectionEdit< T > > handleDragDrop( uint32_t i,
                                                       uint32_t entryCount,
                                                       float32 rowTop,
                                                       float32 rowBottom,
                                                       float32 rowLeft,
                                                       float32 rowRight,
                                                       const InplaceString< 32 >& payloadType ) {
    const ImGuiStyle& style = ImGui::GetStyle();
    const ImGuiPayload* payload = ImGui::GetDragDropPayload();
    bool payloadMatches = payload != nullptr && payload->IsDataType( payloadType.getData() );
    if( !payloadMatches ) {
        return std::nullopt;
    }

    uint32_t sourceIndex = *static_cast< const uint32_t* >( payload->Data );
    float mouseY = ImGui::GetMousePos().y;
    bool insertAbove = mouseY < ( rowTop + rowBottom ) * 0.5f;
    uint32_t targetIndex = insertAbove ? i : i + 1;

    bool isValidDropTarget = ( sourceIndex != targetIndex ) &&       // don't allow drop on same index
                             ( sourceIndex != i ) &&                 // or on drag start source
                             ( ( sourceIndex + 1 ) != targetIndex ); // or top of the item below source

    if( !isValidDropTarget || !ImGui::BeginDragDropTarget() ) {
        return std::nullopt;
    }

    float lineY = insertAbove ? rowTop - style.ItemSpacing.y : rowBottom - style.ItemSpacing.y;
    ImGui::GetForegroundDrawList()->AddLine( ImVec2( rowLeft, lineY ),
                                             ImVec2( rowRight, lineY ),
                                             ImGui::GetColorU32( ImGuiCol_DragDropTarget ),
                                             2.0f );

    bool acceptedPayload = ImGui::AcceptDragDropPayload( payloadType.getData() ) != nullptr;
    ImGui::EndDragDropTarget();
    if( acceptedPayload ) {
        if( sourceIndex < targetIndex )
            --targetIndex;

        return CollectionEdit< T >{ CollectionEditType::Move, sourceIndex, targetIndex };
    }

    return std::nullopt;
}

template < typename T >
inline Optional< CollectionEdit< T > > drawContextMenu( StringView id,
                                                        uint32_t i,
                                                        uint32_t entryCount,
                                                        CollectionOptions options ) {
    const bool allowReorder = onyx::enums::all( options.Flags, CollectionFlags::Reorder );
    const bool allowInsert = onyx::enums::all( options.Flags, CollectionFlags::Insert );
    const bool allowDelete = onyx::enums::all( options.Flags, CollectionFlags::Delete );
    if( ( allowReorder == false ) && ( allowInsert == false ) && ( allowDelete == false ) )
        return std::nullopt;

    ImGui::OpenPopupOnItemClick( id.data(), ImGuiPopupFlags_MouseButtonRight );

    if( ImGui::BeginPopup( id.data() ) ) {
        auto drawContextMenuItems = [ & ]() -> Optional< CollectionEdit< T > > {
            if( allowInsert ) {
                if( ui::selectable( onyx::localization::generic::InsertBefore, false ) ) {
                    return details::CollectionEdit< T >{ details::CollectionEditType::Insert, i };
                }
                if( ui::selectable( onyx::localization::generic::InsertAfter, false ) ) {
                    return details::CollectionEdit< T >{ details::CollectionEditType::Insert, i + 1 };
                }
            }

            if( allowReorder ) {
                {
                    ScopedImGuiDisabled disable( i == 0 );
                    if( ui::selectable( onyx::localization::generic::MoveUp, false ) ) {
                        return details::CollectionEdit< T >{ details::CollectionEditType::Move, i, i - 1 };
                    }
                }

                {
                    ScopedImGuiDisabled disable( ( i + 1 ) >= entryCount );
                    if( ui::selectable( onyx::localization::generic::MoveDown, false ) ) {
                        return details::CollectionEdit< T >{ details::CollectionEditType::Move, i, i + 1 };
                    }
                }
            }

            if( ui::selectable( onyx::localization::generic::Delete, false ) ) {
                return details::CollectionEdit< T >{ details::CollectionEditType::Delete, i };
            }

            return std::nullopt;
        };

        Optional< CollectionEdit< T > > editAction = drawContextMenuItems();
        ImGui::EndPopup();
        return editAction;
    }

    return std::nullopt;
}

template < typename T, typename F > requires std::is_invocable_r_v< bool, F, StringView, T& >
inline Optional< CollectionEdit< T > > handleAppend( F functor ) {
    StringView addLabel = onyx::localization::generic::AddCollectionEntry.Get();
    {
        ScopedImGuiDisabled disable;
        drawPropertyName( addLabel );
    }

    T newValue{};
    if( functor( addLabel, newValue ) ) {
        details::CollectionEdit< T > appendEdit;
        appendEdit.Type = details::CollectionEditType::Append;
        appendEdit.Value = newValue;
        return appendEdit;
    }

    return std::nullopt;
}

template < typename T, typename F >
concept DrawEntryCallable = std::is_invocable_r_v< bool, F, StringView, T& >;

template < typename T, typename DrawEntryFn, typename DrawAppendEntryFn >
requires DrawEntryCallable< T, DrawEntryFn > && DrawEntryCallable< T, DrawAppendEntryFn >
bool drawCollectionProperty( StringView propertyName,
                             DynamicArray< T >& inOutCollection,
                             CollectionOptions options,
                             DrawEntryFn drawEntryFunctor,
                             DrawAppendEntryFn drawAppendEntryFunctor ) {
    ImVec2 valueStartPosition = internal::getPropertyValuePosition();
    ScopedImGuiId id( propertyName );

    ScopedImGuiColor color{ { ImGuiCol_FrameBg, 0 }, { ImGuiCol_HeaderActive, 0 }, { ImGuiCol_HeaderHovered, 0 } };
    const ImGuiStyle& style = ImGui::GetStyle();
    ScopedImGuiStyle styleOverride{
        { ImGuiStyleVar_FramePadding, ImVec2( 0.0f, style.FramePadding.y ) },
    };

    const bool allowReorder = onyx::enums::all( options.Flags, CollectionFlags::Reorder );
    const bool allowAppend = onyx::enums::all( options.Flags, CollectionFlags::Append );
    const bool allowDelete = onyx::enums::all( options.Flags, CollectionFlags::Delete );

    const ImGuiID collapsibleId = ImGui::GetID( propertyName.data() );
    bool isOpen = ImGui::TreeNodeBehavior( collapsibleId,
                                           ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_NoTreePushOnOpen |
                                               ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_NoAutoOpenOnLog,
                                           propertyName.data() );
    ImVec2 space = valueStartPosition - ImVec2( ImGui::GetItemRectSize().x - style.ItemSpacing.x, 0.0f );
    ImGui::SameLine();
    ImGui::Dummy( space );
    ImGui::SameLine();

    InplaceString< 32 > entriesLabel;
    format::formatTo2( entriesLabel, onyx::localization::generic::CollectionSize.Get(), inOutCollection.size() );
    ImGui::TextEx( entriesLabel.getData() );

    if( isOpen == false ) {
        return false;
    }

    color.Reset();
    styleOverride.Reset();

    Optional< details::CollectionEdit< T > > editActionOptional;

    InplaceString< 32 > payloadIdentifier;
    if( allowReorder ) {
        payloadIdentifier = details::makeReorderPayloadType( inOutCollection );
    }

    ScopedImGuiIndent indent( 12.0f );
    uint32_t index = 0;
    ImGui::BeginVertical( "##entries" );
    {
        uint32_t entryCount = numericCast< uint32_t >( inOutCollection.size() );
        for( uint32_t i = 0; i < entryCount; ++i ) {
            InplaceString< 32 > entryName;
            format::formatTo2( entryName, onyx::localization::generic::CollectionEntry.Get(), index++ );

            const float32 rowTop = ImGui::GetCursorScreenPos().y;
            bool isFocused = false;
            {
                ScopedImGuiId entryScope( i );
                drawPropertyName( entryName.getData() );
                isFocused = ImGui::IsItemFocused();

                if( allowReorder && ImGui::BeginDragDropSource( ImGuiDragDropFlags_None ) ) {
                    ImGui::SetDragDropPayload( payloadIdentifier.getData(), &i, sizeof( uint32_t ) );
                    ImGui::TextEx( entryName.getData(), entryName.getData() + entryName.getLength() );
                    ImGui::EndDragDropSource();
                }

                drawEntryFunctor( entryName.getData(), inOutCollection[ i ] );
                isFocused |= ImGui::IsItemFocused();
            }
            ImGui::EndHorizontal();

            if( allowReorder ) {
                const float32 rowLeft = ImGui::GetItemRectMin().x;
                const float32 rowRight = ImGui::GetItemRectMax().x;
                const float32 rowBottom = ImGui::GetCursorScreenPos().y;

                editActionOptional = details::handleDragDrop< T >( i,
                                                                   entryCount,
                                                                   rowTop,
                                                                   rowBottom,
                                                                   rowLeft,
                                                                   rowRight,
                                                                   payloadIdentifier );

                if( editActionOptional.has_value() )
                    break;
            }

            if( allowDelete && isFocused && ImGui::IsKeyPressed( ImGuiKey_Delete ) ) {
                editActionOptional = details::CollectionEdit< T >{ details::CollectionEditType::Delete, i };
                break;
            }

            editActionOptional = details::drawContextMenu< T >( entryName.stringView(), i, entryCount, options );
            if( editActionOptional.has_value() )
                break;
        }

        if( ( editActionOptional.has_value() == false ) && allowAppend ) {
            editActionOptional = details::handleAppend< T >( drawAppendEntryFunctor );

            ImGui::EndHorizontal();
        }

        ImGui::EndVertical();

        // ImGui::EndGroup();
        if( editActionOptional.has_value() ) {
            const details::CollectionEdit< T >& editAction = *editActionOptional;
            details::applyEdit( editAction, inOutCollection );
            return true;
        }
        return false;
    }
}

} // namespace details

inline bool drawProperty( StringView propertyName, DynamicArray< String >& collection ) {
    auto drawEntryFunctor = [ & ]( StringView id, String& value ) { return internal::drawPropertyValue( id, value ); };

    return details::drawCollectionProperty( propertyName, collection, {}, drawEntryFunctor, drawEntryFunctor );
}

inline bool drawProperty( StringView propertyName, DynamicArray< StringView >& collection ) {
    auto drawEntryFunctor = [ & ]( StringView id, StringView value ) {
        return internal::drawPropertyValue( id, value );
    };

    return details::drawCollectionProperty( propertyName, collection, {}, drawEntryFunctor, drawEntryFunctor );
}

template < typename T >
bool drawProperty( StringView propertyName, DynamicArray< T >& collection ) {
    auto drawEntryFunctor = [ & ]( StringView id, T& value ) { return internal::drawPropertyValue( id, value ); };

    return details::drawCollectionProperty( propertyName, collection, {}, drawEntryFunctor, drawEntryFunctor );
}

template < typename T > requires std::is_base_of_v< assets::AssetInterface, T >
bool drawProperty( StringView propertyName, DynamicArray< assets::AssetHandle< T > >& collection ) {
    auto drawEntryFunctor = [ & ]( StringView id, assets::AssetHandle< T >& value ) {
        return internal::drawPropertyValue( id, value );
    };

    return details::drawCollectionProperty( propertyName, collection, {}, drawEntryFunctor, drawEntryFunctor );
}

template < typename ScalarT > requires( std::is_arithmetic_v< ScalarT > )
bool drawProperty( StringView propertyName, DynamicArray< ScalarT >& collection ) {
    auto drawEntryFunctor = [ & ]( StringView id, ScalarT& value ) {
        return internal::drawPropertyValue( id, value, ScalarInputFlag::None, {}, ImGuiInputTextFlags_None );
    };

    auto drawAppendEntryFunctor = [ & ]( StringView id, ScalarT& value ) {
        return internal::drawPropertyValue( id,
                                            value,
                                            ScalarInputFlag::None,
                                            {},
                                            ImGuiInputTextFlags_DisplayEmptyRefVal );
    };

    return details::drawCollectionProperty( propertyName, collection, {}, drawEntryFunctor, drawAppendEntryFunctor );
}

} // namespace onyx::ui::property_grid

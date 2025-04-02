set(TARGET_PUBLIC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/public/editor)
set(TARGET_PRIVATE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/private/editor)

set(TARGET_PUBLIC_SOURCES
	${TARGET_PUBLIC_PATH}/editormodule.h
	${TARGET_PUBLIC_PATH}/camera/freecamera.h
	${TARGET_PUBLIC_PATH}/assets/importer/assetimporter.h
	${TARGET_PUBLIC_PATH}/assets/importer/textureimporter.h
	${TARGET_PUBLIC_PATH}/nodegraph/grapheditorcontext.h
	${TARGET_PUBLIC_PATH}/nodegraph/nodegrapheditorcontext.h
	${TARGET_PUBLIC_PATH}/nodegraph/rendergrapheditorcontext.h
	${TARGET_PUBLIC_PATH}/nodegraph/shadergrapheditorcontext.h
	${TARGET_PUBLIC_PATH}/nodegraph/typenodegrapheditorcontext.h
	${TARGET_PUBLIC_PATH}/modals/assetselectionmodal.h
	${TARGET_PUBLIC_PATH}/modules/nodeeditor.h
	${TARGET_PUBLIC_PATH}/modules/sceneeditor.h
	${TARGET_PUBLIC_PATH}/panels/contentbrowserpanel.h
	${TARGET_PUBLIC_PATH}/panels/nodeeditor/canvaspanel.h
	${TARGET_PUBLIC_PATH}/panels/nodeeditor/propertiespanel.h
	${TARGET_PUBLIC_PATH}/panels/nodeeditor/styles/blueprint.h
	${TARGET_PUBLIC_PATH}/panels/nodeeditor/styles/drawing.h
	${TARGET_PUBLIC_PATH}/panels/nodeeditor/styles/widgets.h
	${TARGET_PUBLIC_PATH}/panels/sceneeditor/entitiespanel.h
	${TARGET_PUBLIC_PATH}/panels/sceneeditor/componentspanel.h
	${TARGET_PUBLIC_PATH}/rendertasks/compositetask.h
	${TARGET_PUBLIC_PATH}/rendertasks/gridtask.h
	${TARGET_PUBLIC_PATH}/rendertasks/guitask.h
	${TARGET_PUBLIC_PATH}/windows/editorwindow.h
	${TARGET_PUBLIC_PATH}/windows/editormainwindow.h	
	${TARGET_PUBLIC_PATH}/windows/startupwindow.h
	${TARGET_PUBLIC_PATH}/windows/settings/inputactionsettingswindow.h
)

set(TARGET_PRIVATE_SOURCES
	${TARGET_PRIVATE_PATH}/editormodule.cpp
	${TARGET_PRIVATE_PATH}/camera/freecamera.cpp
	${TARGET_PRIVATE_PATH}/assets/importer/textureimporter.cpp
	${TARGET_PRIVATE_PATH}/components/cameracomponent.cpp
	${TARGET_PRIVATE_PATH}/components/freecameracomponent_editor.cpp
	${TARGET_PRIVATE_PATH}/components/idcomponent.cpp
	${TARGET_PRIVATE_PATH}/components/namecomponent.cpp
	${TARGET_PRIVATE_PATH}/components/transformcomponent.cpp
	${TARGET_PRIVATE_PATH}/components/graphics/lightcomponents.cpp
	${TARGET_PRIVATE_PATH}/components/graphics/materialcomponent.cpp
	${TARGET_PRIVATE_PATH}/components/graphics/textcomponent_editor.cpp
	${TARGET_PRIVATE_PATH}/components/volume/volumecomponent_editor.cpp
	${TARGET_PRIVATE_PATH}/nodegraph/vectornodes_editor.cpp
	${TARGET_PRIVATE_PATH}/nodegraph/grapheditorcontext.cpp
	${TARGET_PRIVATE_PATH}/nodegraph/nodegrapheditorcontext.cpp
	${TARGET_PRIVATE_PATH}/nodegraph/pin.cpp
	${TARGET_PRIVATE_PATH}/nodegraph/rendergrapheditorcontext.cpp
	${TARGET_PRIVATE_PATH}/nodegraph/rendergraphnodes_editor.cpp
	${TARGET_PRIVATE_PATH}/nodegraph/shadergrapheditorcontext.cpp
	${TARGET_PRIVATE_PATH}/modals/assetselectionmodal.cpp
	${TARGET_PRIVATE_PATH}/modules/nodeeditor.cpp
	${TARGET_PRIVATE_PATH}/modules/sceneeditor.cpp
	${TARGET_PRIVATE_PATH}/panels/contentbrowserpanel.cpp
	${TARGET_PRIVATE_PATH}/panels/nodeeditor/canvaspanel.cpp
	${TARGET_PRIVATE_PATH}/panels/nodeeditor/propertiespanel.cpp
	${TARGET_PRIVATE_PATH}/panels/nodeeditor/styles/blueprint.cpp
	${TARGET_PRIVATE_PATH}/panels/nodeeditor/styles/drawing.cpp
	${TARGET_PRIVATE_PATH}/panels/nodeeditor/styles/widgets.cpp
	${TARGET_PRIVATE_PATH}/panels/sceneeditor/entitiespanel.cpp
	${TARGET_PRIVATE_PATH}/panels/sceneeditor/componentspanel.cpp
	${TARGET_PRIVATE_PATH}/rendertasks/compositetask.cpp
	${TARGET_PRIVATE_PATH}/rendertasks/gridtask.cpp
	${TARGET_PRIVATE_PATH}/rendertasks/guitask.cpp
	${TARGET_PRIVATE_PATH}/shadergraph/sampletexturenode_editor.cpp
	${TARGET_PRIVATE_PATH}/windows/editorwindow.cpp
	${TARGET_PRIVATE_PATH}/windows/editormainwindow.cpp
	${TARGET_PRIVATE_PATH}/windows/startupwindow.cpp
	${TARGET_PRIVATE_PATH}/windows/settings/inputactionsettingswindow.cpp
)
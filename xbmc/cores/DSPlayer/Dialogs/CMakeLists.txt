set(SOURCES GUIDialogDSFilters.cpp
            GUIDialogDSManager.cpp
            GUIDialogDSPlayercoreFactory.cpp
            GUIDialogDSRules.cpp
            GUIDialogLAVAudio.cpp
            GUIDialogLAVSplitter.cpp
            GUIDialogLAVVideo.cpp
            GUIDialogMadvrSettings.cpp
            GUIDialogMadvrSettingsBase.cpp)

set(HEADERS GUIDialogDSFilters.h
            GUIDialogDSManager.h
            GUIDialogDSPlayercoreFactory.h
            GUIDialogDSRules.h
            GUIDialogLAVAudio.h
            GUIDialogLAVSplitter.h
            GUIDialogLAVVideo.h
            GUIDialogMadvrSettings.h
            GUIDialogMadvrSettingsBase.h)

core_add_library(Dialogs)

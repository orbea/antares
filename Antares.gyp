{
    'target_defaults': {
        'include_dirs': [
            'include',
            'stubs',
            '<(DEPTH)/ext/libpng',
            '<(DEPTH)/ext/librezin/include',
            '<(DEPTH)/ext/librgos/include',
            '<(DEPTH)/ext/libsfz/include',
        ],
        'xcode_settings': {
            'GCC_TREAT_WARNINGS_AS_ERRORS': 'YES',
            'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
            'SDKROOT': 'macosx10.4',
            'GCC_VERSION': '4.0',
            'GCC_PREPROCESSOR_DEFINITIONS': [
                'TARGET_OS_MAC',
                'powerc',
            ],
            'ARCHS': 'ppc i386',
            'WARNING_CFLAGS': [
                '-Wall',
                '-Wendif-labels',
            ],
        },
    },
    'targets': [
        {
            'target_name': 'AntaresTest',
            'type': 'executable',
            'sources': [
                'src/TestVideoDriver.cpp',
                'src/TestMain.cpp',
            ],
            'dependencies': [
                'libAntares',
            ],
        },
        {
            'target_name': 'BuildPixTest',
            'type': 'executable',
            'include_dirs': [
                '<(DEPTH)/ext/googletest/include',
                '<(DEPTH)/ext/googlemock/include',
            ],
            'sources': [
                'src/BuildPixMain.cpp',
            ],
            'dependencies': [
                'libAntares',
                '<(DEPTH)/ext/googlemock/googlemock.gyp:gmock_main',
            ],
        },
        {
            'target_name': 'Antares',
            'type': 'executable',
            'mac_bundle': 1,
            'sources': [
                'src/AntaresController.mm',
                'src/CocoaMain.mm',
                'src/CocoaVideoDriver.mm',
                'src/CocoaPrefsDriver.mm',
            ],
            'dependencies': [
                'libAntares',
            ],
            'xcode_settings': {
                'INFOPLIST_FILE': 'resources/Antares-Info.plist',
            },
            'mac_bundle_resources': [
                'resources/Antares.icns',
                'resources/MainMenu.nib',
                'data',
            ],
            'link_settings': {
                'libraries': [
                    '$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
                    '$(SDKROOT)/System/Library/Frameworks/OpenGL.framework',
                ],
            },
        },
        {
            'target_name': 'libAntares',
            'type': '<(library)',
            'sources': [
                'src/Admiral.cpp',
                'src/AnyCharStringHandling.cpp',
                'src/AresCheat.cpp',
                'src/AresGlobalType.cpp',
                'src/AresMain.cpp',
                'src/Beam.cpp',
                'src/BriefingRenderer.cpp',
                'src/BriefingScreen.cpp',
                'src/BuildPix.cpp',
                'src/Card.cpp',
                'src/CardStack.cpp',
                'src/ColorTable.cpp',
                'src/ColorTranslation.cpp',
                'src/DebriefingScreen.cpp',
                'src/DirectText.cpp',
                'src/EZSprite.cpp',
                'src/Error.cpp',
                'src/Event.cpp',
                'src/FakeDrawing.cpp',
                'src/FakeHandles.cpp',
                'src/FakeMath.cpp',
                'src/FakeSounds.cpp',
                'src/Fakes.cpp',
                'src/File.cpp',
                'src/Geometry.cpp',
                'src/HelpScreen.cpp',
                'src/ImageDriver.cpp',
                'src/InputSource.cpp',
                'src/Instruments.cpp',
                'src/InterfaceHandling.cpp',
                'src/InterfaceScreen.cpp',
                'src/InterfaceText.cpp',
                'src/KeyMapTranslation.cpp',
                'src/Ledger.cpp',
                'src/LibpngImageDriver.cpp',
                'src/LoadingScreen.cpp',
                'src/MainScreen.cpp',
                'src/MathSpecial.cpp',
                'src/MessageScreen.cpp',
                'src/Minicomputer.cpp',
                'src/Motion.cpp',
                'src/NateDraw.cpp',
                'src/NatePixTable.cpp',
                'src/NonPlayerShip.cpp',
                'src/OffscreenGWorld.cpp',
                'src/OptionsScreen.cpp',
                'src/Picture.cpp',
                'src/PixMap.cpp',
                'src/PlayAgainScreen.cpp',
                'src/PlayerInterface.cpp',
                'src/PlayerInterfaceDrawing.cpp',
                'src/PlayerShip.cpp',
                'src/Preferences.cpp',
                'src/PrefsDriver.cpp',
                'src/Races.cpp',
                'src/Randomize.cpp',
                'src/ReplayGame.cpp',
                'src/Resource.cpp',
                'src/RetroText.cpp',
                'src/Rotation.cpp',
                'src/ScenarioData.cpp',
                'src/ScenarioMaker.cpp',
                'src/ScreenLabel.cpp',
                'src/SelectLevelScreen.cpp',
                'src/ScrollStars.cpp',
                'src/ScrollTextScreen.cpp',
                'src/SoloGame.cpp',
                'src/SoundFX.cpp',
                'src/SpaceObjectHandling.cpp',
                'src/SpriteCursor.cpp',
                'src/SpriteHandling.cpp',
                'src/StringHandling.cpp',
                'src/StringList.cpp',
                'src/StringNumerics.cpp',
                'src/Time.cpp',
                'src/Transitions.cpp',
                'src/VideoDriver.cpp',
            ],
            'dependencies': [
                '<(DEPTH)/ext/libpng/libpng.gyp:libpng',
                '<(DEPTH)/ext/librezin/librezin.gyp:librezin',
                '<(DEPTH)/ext/librgos/librgos.gyp:librgos',
                '<(DEPTH)/ext/libsfz/libsfz.gyp:libsfz',
            ],
        },
    ],
}

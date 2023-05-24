CMAKE_MINIMUM_REQUIRED(VERSION 3.5.2)

PROJECT(capture CXX)
SET(CMAKE_CXX_STANDARD 14)

# Option to set path for DeckLink SDK
# if (NOT DEFINED DECKLINK_SDK_INCLUDE_DIR)
#         SET(DECKLINK_SDK_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
# endif ()


set(DECKLINK_SDK_INCLUDE_DIR /home/meysam/Downloads/BlackMagic/Blackmagic_DeckLink_SDK_12.4.2/Linux/include)

    # Add DeckLink API
SET(PLATFORM_SOURCES
        ${DECKLINK_SDK_INCLUDE_DIR}/DeckLinkAPI.h
        ${DECKLINK_SDK_INCLUDE_DIR}/DeckLinkAPIDispatch.cpp
        )

LIST(APPEND PLATFORM_LIBRARIES dl pthread)

SET(APP_HEADERS
        capture_delegate.h
        Config.h
        bmcapture.h
        )

SET(APP_SOURCES
        main.cpp
        Config.cpp
        capture_delegate.cpp
        bmcapture.cpp
        ${APP_HEADERS}
        )

ADD_EXECUTABLE(${PROJECT_NAME}
        ${APP_SOURCES}
        ${PLATFORM_SOURCES}
        )

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE
        ${DECKLINK_SDK_INCLUDE_DIR}
        )

# ADD_DEFINITIONS(-DOPENCV_DIR=${OpenCV_DIR}) # Used at runtime to load the cascade classifier XML file

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        ${PLATFORM_LIBRARIES}
        )


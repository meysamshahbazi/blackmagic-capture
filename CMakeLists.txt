CMAKE_MINIMUM_REQUIRED(VERSION 3.5.2)

PROJECT(capture CXX)
SET(CMAKE_CXX_STANDARD 14)

# Option to set path for DeckLink SDK
if (NOT DEFINED DECKLINK_SDK_INCLUDE_DIR)
        SET(DECKLINK_SDK_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif ()


set(DECKLINK_SDK_INCLUDE_DIR /home/meysam/Downloads/BlackMagic/Blackmagic_DeckLink_SDK_12.4.2/Linux/include)

    # Add DeckLink API
SET(PLATFORM_SOURCES
        ${DECKLINK_SDK_INCLUDE_DIR}/DeckLinkAPI.h
        ${DECKLINK_SDK_INCLUDE_DIR}/DeckLinkAPIDispatch.cpp
        )

LIST(APPEND PLATFORM_LIBRARIES dl pthread)


SET(APP_HEADERS
        com_ptr.h
        com_utils.h
        DeviceManager.h
        LiveVideoWithOpenCV.h
        )

SET(APP_SOURCES
        LiveVideoWithOpenCV.cpp
        ${APP_HEADERS}
        )

ADD_EXECUTABLE(LiveVideoWithOpenCV
        ${APP_SOURCES}
        ${PLATFORM_SOURCES}
        )

IF (WIN32)
    # Compile DeckLink API IDL to build directory
    SET(MIDL_OUTPUT_SOURCE "${CMAKE_CURRENT_BINARY_DIR}\\DeckLinkAPI_i.c")
    SET(MIDL_OUTPUT_HEADER "${CMAKE_CURRENT_BINARY_DIR}\\DeckLinkAPI.h")
    SET(DECKLINK_API_IDL "${DECKLINK_SDK_INCLUDE_DIR}\\DeckLinkAPI.idl")
    SET(DECKLINK_SDK_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR})

    IF (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)")
        SET(CSHARP_PLATFORM x86)
        SET(MIDL_MACHINE win32)
    ELSE ()
        SET(CSHARP_PLATFORM x64)
        SET(MIDL_MACHINE amd64)
    ENDIF ()

    SET(MIDL_OUTPUT
            ${MIDL_OUTPUT_SOURCE}
            ${MIDL_OUTPUT_HEADER}
    )

    # Compile the DeckLinkAPI.idl file (to generate DeckLinkAPI.h and DeckLinkAPI_i.c)
    ADD_CUSTOM_COMMAND(
            OUTPUT ${MIDL_OUTPUT}
            DEPENDS ${DECKLINK_API_IDL}
            COMMAND midl /env ${MIDL_MACHINE} /header ${MIDL_OUTPUT_HEADER} ${DECKLINK_API_IDL}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    SET_SOURCE_FILES_PROPERTIES(${MIDL_OUTPUT} PROPERTIES GENERATED TRUE)
ENDIF ()

TARGET_INCLUDE_DIRECTORIES(LiveVideoWithOpenCV PRIVATE
        ${DECKLINK_SDK_INCLUDE_DIR}
        ${OpenCV_INCLUDE_DIRS}
        )

ADD_DEFINITIONS(-DOPENCV_DIR=${OpenCV_DIR}) # Used at runtime to load the cascade classifier XML file

TARGET_LINK_LIBRARIES(LiveVideoWithOpenCV
        ${OpenCV_LIBS}
        ${PLATFORM_LIBRARIES}
        )


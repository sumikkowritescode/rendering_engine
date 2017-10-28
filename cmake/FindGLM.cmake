IF(WIN32)
    SET(GLM_HEADER_SEARCH_DIRS
    "${CMAKE_SOURCE_DIR}/includes"
    )
ELSE(WIN32)
    SET(GLM_HEADER_SEARCH_DIRS
        "/usr/include"
        "/usr/local/include"
    )
ENDIF(WIN32)

# check environment variable
SET(GLM_ENV_ROOT_DIR "$ENV{GLM_ROOT_DIR}")

IF(NOT GLM_ROOT_DIR AND GLM_ENV_ROOT_DIR)
    SET(GLM_ROOT_DIR "${GLM_ENV_ROOT_DIR}")
ENDIF(NOT GLM_ROOT_DIR AND GLM_ENV_ROOT_DIR)

# put user specified location at beginning of search
IF(GLM_ROOT_DIR)
    SET(GLM_HEADER_SEARCH_DIRS "${GLM_ROOT_DIR}"
                                "${GLM_ROOT_DIR}/include"
                                 ${GLM_HEADER_SEARCH_DIRS})
ENDIF(GLM_ROOT_DIR)

# locate header
FIND_PATH(GLM_INCLUDE_DIR "glm/glm.hpp"
    PATHS ${GLM_HEADER_SEARCH_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLM DEFAULT_MSG
    GLM_INCLUDE_DIR)

IF(GLM_FOUND)
    SET(GLM_INCLUDE_DIRS "${GLM_INCLUDE_DIR}")

    MESSAGE(STATUS "GLM_INCLUDE_DIR = ${GLM_INCLUDE_DIR}")
ENDIF(GLM_FOUND)

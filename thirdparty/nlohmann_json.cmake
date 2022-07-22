include (ExternalProject)

SET(PREFIX Assimp)
SET(GIT_URL https://github.com/assimp/assimp.git)
SET(GIT_TAG v4.1.0)

SET(JSON_INSTALL_DIR ${CMAKE_BINARY_DIR}/externals/ CACHE PATH "" FORCE)

ExternalProject_Add(nlohmann_json
  GIT_REPOSITORY "https://github.com/nlohmann/json.git"
  GIT_TAG "v3.6.1"
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CMAKE_ARGS -DJSON_BuildTests=OFF -DJSON_MultipleHeaders=OFF -DCMAKE_INSTALL_PREFIX=${JSON_INSTALL_DIR}
  TEST_COMMAND ""
)

SET(JSON_INCLUDE_DIR ${JSON_INSTALL_DIR}/include CACHE PATH "")

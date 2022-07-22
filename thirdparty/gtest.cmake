include (ExternalProject)

SET(PREFIX googletest)
SET(GIT_URL https://github.com/google/googletest.git)
SET(GIT_TAG "release-1.8.1")

SET(GTEST_INSTALL_DIR ${CMAKE_BINARY_DIR}/externals/)

ExternalProject_Add(${PREFIX}
  GIT_REPOSITORY "${GIT_URL}"
  GIT_TAG "${GIT_TAG}"
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${GTEST_INSTALL_DIR}
  TEST_COMMAND ""
)

# MSVC only 
SET(GTEST_INCLUDE_DIR "${GTEST_INSTALL_DIR}/include" CACHE PATH "")
SET(GTEST_LIB_DIR "${GTEST_INSTALL_DIR}/lib" CACHE PATH "" )
SET(GTEST_TEST_LIBRARY
    optimized   gtest.lib
    debug       gtestd.lib
    optimized   gtest_main.lib  
    debug       gtest_maind.lib 
    CACHE INTERNAL "" FORCE
)
SET(GTEST_MOCK_LIBRARY
    optimized   gmock.lib
    debug       gmockd.lib
    optimized   gmock_main.lib  
    debug       gmock_maind.lib 
    CACHE INTERNAL "" FORCE
)
SET(GTEST_BOTH_LIBRARIES ${GTEST_TEST_LIBRARY} ${GTEST_MOCK_LIBRARY} CACHE INTERNAL "" FORCE )
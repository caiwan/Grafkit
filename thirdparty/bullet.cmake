include (ExternalProject)

ExternalProject_Add(Bullet3
  GIT_REPOSITORY "https://github.com/bulletphysics/bullet3.git"
  GIT_TAG "2.87"
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
  
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  
  CMAKE_ARGS -DBUILD_EXTRAS=OFF -DBUILD_BULLET2_DEMOS=OFF -DBUILD_PYBULLET=OFF -DBUILD_UNIT_TESTS=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external/bullet3
  
  TEST_COMMAND ""
)

ExternalProject_Get_Property(Bullet3 install_dir)

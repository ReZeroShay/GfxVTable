add_executable(GfxVTableExample main.cpp)

target_link_libraries(GfxVTableExample PRIVATE GfxVTable::GfxVTable)

# The backend header pulls in d3dcompiler for runtime shader compilation.
target_link_libraries(GfxVTableExample PRIVATE d3d9.lib d3dcompiler.lib)

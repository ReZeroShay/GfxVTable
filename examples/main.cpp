#include <GfxVTable/GfxVTable.h>
#include <GfxVTable/D3D11VTable.h>

int main() {
    GfxVTable::VTableForD3D11([](void **, void **, void **) {});
    return 0;
}

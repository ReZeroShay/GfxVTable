#pragma once
#include <concepts>
#include <functional>
#if !defined(WIN32_LEAN_AND_MEAN)
#    define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX)
#    define NOMINMAX
#endif
#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>

#include <GfxVTable/Lib.h>
#include <GfxVTable/GfxVTable.h>

namespace GfxVTable {
    using CreateDXGIFactoryType = HRESULT(WINAPI *)(REFIID riid, void **ppFactory);

    using D3D11CreateDeviceAndSwapChainType = HRESULT(WINAPI *)(IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT,
                                                                const D3D_FEATURE_LEVEL *, UINT, UINT,
                                                                const DXGI_SWAP_CHAIN_DESC *, IDXGISwapChain **,
                                                                ID3D11Device **, D3D_FEATURE_LEVEL *,
                                                                ID3D11DeviceContext **);
    enum D3D11ErrorCode {
        None,
        ModuleNotFound,
        MethodNotFound,
        CreateDXGIFactoryFailed,
        EnumAdaptersFailed,
        CreateDeviceAndSwapChainFailed,
        DirectX,
        Vulkan2
    };

    template <typename F>
        requires std::invocable<F, void **, void **, void **>
    inline auto VTableForD3D11(F &&fn) -> D3D11ErrorCode {
        auto dxgi_dll = GetModuleHandleA("dxgi.dll");
        if (!dxgi_dll) {
            return ModuleNotFound;
        }

        auto d3d11_dll = GetModuleHandleA("d3d11.dll");
        if (!d3d11_dll) {
            return ModuleNotFound;
        }

        auto CreateDXGIFactory = ((CreateDXGIFactoryType)GetProcAddress(dxgi_dll, "CreateDXGIFactory"));
        if (!CreateDXGIFactory) {
            return MethodNotFound;
        }

        IDXGIFactory *factory;
        auto hresult = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&factory);
        if (hresult != S_OK) {
            return CreateDXGIFactoryFailed;
        }
        ScopeGuard factoryGuard([&]() { factory->Release(); });

        IDXGIAdapter *adapter;
        hresult = factory->EnumAdapters(0, &adapter);
        if (hresult != S_OK) {
            return EnumAdaptersFailed;
        }
        ScopeGuard adapterGuard([&]() { adapter->Release(); });

        auto D3D11CreateDeviceAndSwapChain =
            ((D3D11CreateDeviceAndSwapChainType)GetProcAddress(d3d11_dll, "D3D11CreateDeviceAndSwapChain"));
        if (!D3D11CreateDeviceAndSwapChain) {
            return MethodNotFound;
        }

        WNDCLASSEXA wc = {};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = DefWindowProcA;
        wc.hInstance = GetModuleHandleA(nullptr);
        wc.lpszClassName = "DummyWindow__";

        RegisterClassExA(&wc);

        auto hwnd = CreateWindowExA(0, wc.lpszClassName, "DummyWindow__", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                                    CW_USEDEFAULT, 10, 10, nullptr, nullptr, wc.hInstance, nullptr);
        ScopeGuard windowGuard([&]() {
            DestroyWindow(hwnd);
            UnregisterClassA(wc.lpszClassName, wc.hInstance);
        });

        DXGI_SWAP_CHAIN_DESC sc_desc = {};
        sc_desc.BufferDesc.Width = 100;
        sc_desc.BufferDesc.Height = 100;
        sc_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sc_desc.BufferDesc.RefreshRate.Numerator = 60;
        sc_desc.BufferDesc.RefreshRate.Denominator = 1;
        sc_desc.SampleDesc.Count = 1;
        sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sc_desc.BufferCount = 1;
        sc_desc.OutputWindow = hwnd;
        sc_desc.Windowed = TRUE;
        sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sc_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        const D3D_FEATURE_LEVEL feature_levels[] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };

        IDXGISwapChain *swapchain;
        ID3D11Device *device;
        ID3D11DeviceContext *context;
        D3D_FEATURE_LEVEL feature_level;

        hresult = D3D11CreateDeviceAndSwapChain(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, feature_levels,
                                                ARRAYSIZE(feature_levels), D3D11_SDK_VERSION, &sc_desc, &swapchain,
                                                &device, &feature_level, &context);
        if (hresult != S_OK) {
            return CreateDeviceAndSwapChainFailed;
        }

        ScopeGuard D3D11Guard([&]() {
            swapchain->Release();
            device->Release();
            context->Release();
        });

        std::invoke(fn, *(void ***)swapchain, *(void ***)device, *(void ***)context);
        return None;
    }
} // namespace GfxVTable

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <dlfcn.h>
#include <string>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "hack.h"
#include "log.h"
#include "game.h"
#include "utils.h"
#include "xdl.h"
#include "imgui.h"
#include "imgui_impl_android.h"
#include "imgui_impl_opengl3.h"
#include "MemoryPatch.h"

static int                  g_GlHeight, g_GlWidth;
static bool                 g_IsSetup = false;
static std::string          g_IniFileName = "";
static utils::module_info   g_TargetModule{};

HOOKAF(void, Input, void *thiz, void *ex_ab, void *ex_ac) {
    origInput(thiz, ex_ab, ex_ac);
    ImGui_ImplAndroid_HandleInputEvent((AInputEvent *)thiz);
    return;
}

bool IsLoaded = false;
bool IsVisible = false;
bool IsDebugPanelVisible = false;
bool IsProfilerDocked = false;
bool IsTriggerEnabled= false;

void Patch(void* address, void* pointer, int size) {
	DWORD OldProtection;
	VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &OldProtection);
	memcpy(address, pointer, size);
	VirtualProtect(address, size, OldProtection, &OldProtection);
}

void ReadPointerSatu(DWORD Base, DWORD Ofs, DWORD value) {
	DWORD A = 0;
	if (!IsBadReadPtr((PDWORD)Base, 4)) {
		A = *(PDWORD)((DWORD)(Base)) + Ofs;
		if (!IsBadReadPtr((PDWORD)A, 4)) {
			*(int*)A = value;
		}
	}
}

void SetupImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    io.IniFilename = g_IniFileName.c_str();
    io.DisplaySize = ImVec2((float)g_GlWidth, (float)g_GlHeight);

    ImGui_ImplAndroid_Init(nullptr);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    ImGui::StyleColorsLight();

    ImFontConfig font_cfg;
    font_cfg.SizePixels = 22.0f;
    io.Fonts->AddFontDefault(&font_cfg);

    ImGui::GetStyle().ScaleAllSizes(3.0f);
}

EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &g_GlWidth);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &g_GlHeight);

    if (!g_IsSetup) {
      SetupImGui();
      g_IsSetup = true;
    }

    ImGuiIO &io = ImGui::GetIO();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(g_GlWidth, g_GlHeight);
    ImGui::NewFrame();
    ImGui::Begin("Overdox Cheat By : -E-#4990 </> ");
    ImGui::Checkbox("Is Loaded", &IsLoaded);
	ImGui::Checkbox("Is Visible", &IsVisible);
	ImGui::Checkbox("Is Debug Panel", &IsDebugPanelVisible);
	ImGui::Checkbox("Is Profiler Docked", &IsProfilerDocked);
	ImGui::Checkbox("Is Trigger Enabled", &IsTriggerEnabled);
	ImGui::Text("Overdox Cheat By : -E-#4990 </> ");
    ImGui::EndFrame();
    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return old_eglSwapBuffers(dpy, surface);
}

void hack_start(const char *_game_data_dir) {
    LOGI("hack start | %s", _game_data_dir);
    do {
        sleep(1);
        g_TargetModule = utils::find_module(TargetLibName);
    } while (g_TargetModule.size <= 0);
    LOGI("%s: %p - %p",TargetLibName, g_TargetModule.start_address, g_TargetModule.end_address);

    // TODO: hooking/patching here
    DWORD WINAPI Source(LPVOID param) {
    while (1) {
    	Sleep(47);
    	if (IsLoaded == 1) {
    		Patch((void*)(0x22BAC18), (void*)(PBYTE)"\x20\x00\x80\xD2\xC0\x03\x5F\xD6", 8);
    	}
    	else if (!IsLoaded) {
    		Patch((void*)(0x22BAC18), (void*)(PBYTE)"\xF5\x53\xBE\xA9\xF3\x7B\x01\xA9", 8);
    	}
    	if (IsVisible == 1) {
    		Patch((void*)(0x22BAC78), (void*)(PBYTE)"\x20\x00\x80\xD2\xC0\x03\x5F\xD6", 8);
    	}
    	else if (!IsVisible) {
    		Patch((void*)(0x22BAC78), (void*)(PBYTE)"\xF3\x7B\xBF\xA9\xF3\x03\x00\xAA", 8);
    	}
    	if (IsDebugPanelVisible == 1) {
    		Patch((void*)(0x22C0C24), (void*)(PBYTE)"\x20\x00\x80\xD2\xC0\x03\x5F\xD6", 8);
    	}
    	else if (!IsDebugPanelVisible) {
    		Patch((void*)(0x22C0C24), (void*)(PBYTE)"\xF4\x0F\x1E\xF8\xF3\x7B\x01\xA9", 8);
    	}
    	if (IsProfilerDocked == 1) {
    		Patch((void*)(0x22C0E14), (void*)(PBYTE)"\x20\x00\x80\xD2\xC0\x03\x5F\xD6", 8);
    	}
    	else if (!IsProfilerDocked) {
    		Patch((void*)(0x22C0E14), (void*)(PBYTE)"\xF3\x7B\xBF\xA9\xB3\x83\x00\x90", 8);
    	}
    	if (IsTriggerEnabled == 1) {
    		Patch((void*)(0x22C0CC8), (void*)(PBYTE)"\x20\x00\x80\xD2\xC0\x03\x5F\xD6", 8);
    	}
    	else if (!IsTriggerEnabled) {
    		Patch((void*)(0x22C0CC8), (void*)(PBYTE)"\xF4\x0F\x1E\xF8\xF3\x7B\x01\xA9", 8);
    	}
    	return (0);
    }
}

void hack_prepare(const char *_game_data_dir) {
    LOGI("hack thread: %d", gettid());
    int api_level = utils::get_android_api_level();
    LOGI("api level: %d", api_level);
    g_IniFileName = std::string(_game_data_dir) + "/files/imgui.ini";
    sleep(5);

    void *sym_input = DobbySymbolResolver("/system/lib/libinput.so", "_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE");
    if (NULL != sym_input){
        DobbyHook((void *)sym_input, (dobby_dummy_func_t) myInput, (dobby_dummy_func_t*)&origInput);
    }
    
    void *egl_handle = xdl_open("libEGL.so", 0);
    void *eglSwapBuffers = xdl_sym(egl_handle, "eglSwapBuffers", nullptr);
    if (NULL != eglSwapBuffers) {
        utils::hook((void*)eglSwapBuffers, (func_t)hook_eglSwapBuffers, (func_t*)&old_eglSwapBuffers);
    }
    xdl_close(egl_handle);

    hack_start(_game_data_dir);
}

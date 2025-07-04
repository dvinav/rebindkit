local out_dir = os.getenv("GTA_SA_DIR") or path.join(os.projectdir(), "bin")
local dxsdk = "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)"

add_rules("mode.debug", "mode.release")
set_languages("c++23")
set_kind("shared")
set_targetdir(out_dir)
add_syslinks("user32")
set_arch("x86") 

target("RebindKit")
    add_files("src/RebindKit/*.cpp")

target("dinput8")
    add_files("src/dinput8/dinput8.cpp")
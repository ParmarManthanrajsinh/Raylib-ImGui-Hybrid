#include "ApplicationLayout.h"
#include <string_view>
#include <imgui.h>

namespace Core 
{

// Default docking layout for the editor, loaded on startup
static constexpr std::string_view APPLICATION_DEFAULT_INI = 
R"(
[Window][Debug##Default]
Pos=1193,260
Size=400,400
Collapsed=0

[Window][Controls]
Pos=60,60
Size=360,65
Collapsed=0

[Window][Viewport]
Pos=0,0
Size=1219,900
Collapsed=0
DockId=0x00000001,0

[Window][WindowOverViewport_11111111]
Pos=0,0
Size=1600,900
Collapsed=0

[Window][Settings]
Pos=36,36
Size=435,480
Collapsed=0

[Window][Scene Hierarchy]
Pos=1221,0
Size=379,478
Collapsed=0
DockId=0x00000003,0

[Window][Properties]
Pos=1221,480
Size=379,420
Collapsed=0
DockId=0x00000004,0

[Docking][Data]
DockSpace     ID=0xE098E157 Pos=0,0 Size=1600,900 CentralNode=1 HiddenTabBar=1 Selected=0xC450F867
DockSpace     ID=0xEFEED480 Window=0x1BBC0F80 Pos=0,0 Size=1600,900 Split=X Selected=0xC450F867
  DockNode    ID=0x00000001 Parent=0xEFEED480 SizeRef=1219,900 CentralNode=1 Selected=0xC450F867
  DockNode    ID=0x00000002 Parent=0xEFEED480 SizeRef=379,900 Split=Y Selected=0x8C72BEA8
    DockNode  ID=0x00000003 Parent=0x00000002 SizeRef=258,478 Selected=0xB8729153
    DockNode  ID=0x00000004 Parent=0x00000002 SizeRef=258,420 Selected=0x8C72BEA8
)";

void LoadApplicationDefaultIni()
{
    ImGui::LoadIniSettingsFromMemory
    (
        APPLICATION_DEFAULT_INI.data(), 
        APPLICATION_DEFAULT_INI.size()
    );
}
}

# Claude Code — Project Instructions

## User Preferences

### C++ Brace Style (Allman)
Always use Allman style — opening brace on its own line:
```cpp
void function()
{
    // ...
}

if (condition)
{
    // ...
}
```
Never use K&R style (`function() {`).

### Switch Statement Style
Each `case` block uses braces, with `break` outside the closing brace:
```cpp
switch (x)
{
    case A:
    {
        ...
    }
    break;

    case B:
    {
        ...
    }
    break;
}
```

### Indentation
Use **4 spaces** per indent level. Never use tabs.

### Communication
- User communicates in Traditional Chinese (繁體中文). Respond in the same language.
- Keep responses concise.

## Memory System
This project has a persistent memory system at:
`C:\Users\littl\.claude\projects\T--of-of-v0-11-2-vs2017-release-addons-ofxImGui\memory\`

Key memory files:
- `MEMORY.md` — index of all memories (auto-loaded by Claude Code)
- `project_imgui_upgrade.md` — ImGui 1.83→1.92.8 upgrade notes and breaking changes
- `project_ofxImGuiRemote.md` — ofxImGuiRemote addon architecture and protocol
- `project_ofxImGuiRemote_devlog.md` — dev log: bugs solved, key decisions
- `feedback_code_style.md` — Allman brace style preference

Always check the memory index when starting a new session on this project.

## Project Context
- OF 0.11.2 + ImGui 1.92.8, Visual Studio 2017, Windows 11
- Related addon: [ofxImGuiRemote](https://github.com/UltraCombos/ofxImGuiRemote) — mirrors ImGui UI to browser via WebSocket+WebGL

#pragma once

// Radix UI Color System - Dark mode (solid + alpha scales)
// Full reference: https://www.radix-ui.com/colors
// Each scale has 12 steps:
//   1-2:  Backgrounds
//   3-5:  Interactive component backgrounds
//   6-8:  Borders and separators
//   9-10: Solid colors
//   11:   Accessible text
//   12:   High-contrast text
namespace onyx::colors::dark {

// Overlay (5%)
inline constexpr Color Black1{ 0, 0, 0, 13 };
// Overlay (10%)
inline constexpr Color Black2{ 0, 0, 0, 26 };
// Overlay (15%)
inline constexpr Color Black3{ 0, 0, 0, 38 };
// Overlay (20%)
inline constexpr Color Black4{ 0, 0, 0, 51 };
// Overlay (30%)
inline constexpr Color Black5{ 0, 0, 0, 77 };
// Overlay (40%)
inline constexpr Color Black6{ 0, 0, 0, 102 };
// Overlay (50%)
inline constexpr Color Black7{ 0, 0, 0, 128 };
// Overlay (60%)
inline constexpr Color Black8{ 0, 0, 0, 153 };
// Overlay (70%)
inline constexpr Color Black9{ 0, 0, 0, 179 };
// Overlay (80%)
inline constexpr Color Black10{ 0, 0, 0, 204 };
// Overlay (90%)
inline constexpr Color Black11{ 0, 0, 0, 230 };
// Overlay (95%)
inline constexpr Color Black12{ 0, 0, 0, 242 };

// Overlay (5%)
inline constexpr Color White1{ 255, 255, 255, 13 };
// Overlay (10%)
inline constexpr Color White2{ 255, 255, 255, 26 };
// Overlay (15%)
inline constexpr Color White3{ 255, 255, 255, 38 };
// Overlay (20%)
inline constexpr Color White4{ 255, 255, 255, 51 };
// Overlay (30%)
inline constexpr Color White5{ 255, 255, 255, 77 };
// Overlay (40%)
inline constexpr Color White6{ 255, 255, 255, 102 };
// Overlay (50%)
inline constexpr Color White7{ 255, 255, 255, 128 };
// Overlay (60%)
inline constexpr Color White8{ 255, 255, 255, 153 };
// Overlay (70%)
inline constexpr Color White9{ 255, 255, 255, 179 };
// Overlay (80%)
inline constexpr Color White10{ 255, 255, 255, 204 };
// Overlay (90%)
inline constexpr Color White11{ 255, 255, 255, 230 };
// Overlay (95%)
inline constexpr Color White12{ 255, 255, 255, 242 };

// Background
inline constexpr Color Gray1{ 17, 17, 17, 255 };
// Background (subtle)
inline constexpr Color Gray2{ 25, 25, 25, 255 };
// Interactive background
inline constexpr Color Gray3{ 34, 34, 34, 255 };
// Interactive background (hover)
inline constexpr Color Gray4{ 42, 42, 42, 255 };
// Interactive background (active)
inline constexpr Color Gray5{ 49, 49, 49, 255 };
// Border (subtle)
inline constexpr Color Gray6{ 58, 58, 58, 255 };
// Border
inline constexpr Color Gray7{ 72, 72, 72, 255 };
// Border (strong)
inline constexpr Color Gray8{ 96, 96, 96, 255 };
// Solid
inline constexpr Color Gray9{ 110, 110, 110, 255 };
// Solid (hover)
inline constexpr Color Gray10{ 123, 123, 123, 255 };
// Text
inline constexpr Color Gray11{ 180, 180, 180, 255 };
// Text (high contrast)
inline constexpr Color Gray12{ 238, 238, 238, 255 };

// Alpha background
inline constexpr Color GrayA1{ 0, 0, 0, 0 };
// Alpha background (subtle)
inline constexpr Color GrayA2{ 255, 255, 255, 9 };
// Alpha interactive background
inline constexpr Color GrayA3{ 255, 255, 255, 18 };
// Alpha interactive background (hover)
inline constexpr Color GrayA4{ 255, 255, 255, 27 };
// Alpha interactive background (active)
inline constexpr Color GrayA5{ 255, 255, 255, 34 };
// Alpha border (subtle)
inline constexpr Color GrayA6{ 255, 255, 255, 44 };
// Alpha border
inline constexpr Color GrayA7{ 255, 255, 255, 59 };
// Alpha border (strong)
inline constexpr Color GrayA8{ 255, 255, 255, 85 };
// Alpha solid
inline constexpr Color GrayA9{ 255, 255, 255, 100 };
// Alpha solid (hover)
inline constexpr Color GrayA10{ 255, 255, 255, 114 };
// Alpha text
inline constexpr Color GrayA11{ 255, 255, 255, 175 };
// Alpha text (high contrast)
inline constexpr Color GrayA12{ 255, 255, 255, 237 };

// Background
inline constexpr Color Mauve1{ 18, 17, 19, 255 };
// Background (subtle)
inline constexpr Color Mauve2{ 26, 25, 27, 255 };
// Interactive background
inline constexpr Color Mauve3{ 35, 34, 37, 255 };
// Interactive background (hover)
inline constexpr Color Mauve4{ 43, 41, 45, 255 };
// Interactive background (active)
inline constexpr Color Mauve5{ 50, 48, 53, 255 };
// Border (subtle)
inline constexpr Color Mauve6{ 60, 57, 63, 255 };
// Border
inline constexpr Color Mauve7{ 73, 71, 78, 255 };
// Border (strong)
inline constexpr Color Mauve8{ 98, 95, 105, 255 };
// Solid
inline constexpr Color Mauve9{ 111, 109, 120, 255 };
// Solid (hover)
inline constexpr Color Mauve10{ 124, 122, 133, 255 };
// Text
inline constexpr Color Mauve11{ 181, 178, 188, 255 };
// Text (high contrast)
inline constexpr Color Mauve12{ 238, 238, 240, 255 };

// Alpha background
inline constexpr Color MauveA1{ 0, 0, 0, 0 };
// Alpha background (subtle)
inline constexpr Color MauveA2{ 245, 244, 246, 9 };
// Alpha interactive background
inline constexpr Color MauveA3{ 235, 234, 248, 20 };
// Alpha interactive background (hover)
inline constexpr Color MauveA4{ 238, 229, 248, 29 };
// Alpha interactive background (active)
inline constexpr Color MauveA5{ 239, 230, 254, 37 };
// Alpha border (subtle)
inline constexpr Color MauveA6{ 241, 230, 253, 48 };
// Alpha border
inline constexpr Color MauveA7{ 238, 233, 255, 64 };
// Alpha border (strong)
inline constexpr Color MauveA8{ 238, 231, 255, 93 };
// Alpha solid
inline constexpr Color MauveA9{ 234, 230, 253, 110 };
// Alpha solid (hover)
inline constexpr Color MauveA10{ 236, 233, 253, 124 };
// Alpha text
inline constexpr Color MauveA11{ 245, 241, 255, 183 };
// Alpha text (high contrast)
inline constexpr Color MauveA12{ 253, 253, 255, 239 };

// Background
inline constexpr Color Slate1{ 17, 17, 19, 255 };
// Background (subtle)
inline constexpr Color Slate2{ 24, 25, 27, 255 };
// Interactive background
inline constexpr Color Slate3{ 33, 34, 37, 255 };
// Interactive background (hover)
inline constexpr Color Slate4{ 39, 42, 45, 255 };
// Interactive background (active)
inline constexpr Color Slate5{ 46, 49, 53, 255 };
// Border (subtle)
inline constexpr Color Slate6{ 54, 58, 63, 255 };
// Border
inline constexpr Color Slate7{ 67, 72, 78, 255 };
// Border (strong)
inline constexpr Color Slate8{ 90, 97, 105, 255 };
// Solid
inline constexpr Color Slate9{ 105, 110, 119, 255 };
// Solid (hover)
inline constexpr Color Slate10{ 119, 123, 132, 255 };
// Text
inline constexpr Color Slate11{ 176, 180, 186, 255 };
// Text (high contrast)
inline constexpr Color Slate12{ 237, 238, 240, 255 };

// Alpha background
inline constexpr Color SlateA1{ 0, 0, 0, 0 };
// Alpha background (subtle)
inline constexpr Color SlateA2{ 216, 244, 246, 9 };
// Alpha interactive background
inline constexpr Color SlateA3{ 221, 234, 248, 20 };
// Alpha interactive background (hover)
inline constexpr Color SlateA4{ 211, 237, 248, 29 };
// Alpha interactive background (active)
inline constexpr Color SlateA5{ 217, 237, 254, 37 };
// Alpha border (subtle)
inline constexpr Color SlateA6{ 214, 235, 253, 48 };
// Alpha border
inline constexpr Color SlateA7{ 217, 237, 255, 64 };
// Alpha border (strong)
inline constexpr Color SlateA8{ 217, 237, 255, 93 };
// Alpha solid
inline constexpr Color SlateA9{ 223, 235, 253, 109 };
// Alpha solid (hover)
inline constexpr Color SlateA10{ 229, 237, 253, 123 };
// Alpha text
inline constexpr Color SlateA11{ 241, 247, 254, 181 };
// Alpha text (high contrast)
inline constexpr Color SlateA12{ 252, 253, 255, 239 };

// Background
inline constexpr Color Sage1{ 16, 18, 17, 255 };
// Background (subtle)
inline constexpr Color Sage2{ 23, 25, 24, 255 };
// Interactive background
inline constexpr Color Sage3{ 32, 34, 33, 255 };
// Interactive background (hover)
inline constexpr Color Sage4{ 39, 42, 41, 255 };
// Interactive background (active)
inline constexpr Color Sage5{ 46, 49, 48, 255 };
// Border (subtle)
inline constexpr Color Sage6{ 55, 59, 57, 255 };
// Border
inline constexpr Color Sage7{ 68, 73, 71, 255 };
// Border (strong)
inline constexpr Color Sage8{ 91, 98, 95, 255 };
// Solid
inline constexpr Color Sage9{ 99, 112, 107, 255 };
// Solid (hover)
inline constexpr Color Sage10{ 113, 125, 121, 255 };
// Text
inline constexpr Color Sage11{ 173, 181, 178, 255 };
// Text (high contrast)
inline constexpr Color Sage12{ 236, 238, 237, 255 };

// Alpha background
inline constexpr Color SageA1{ 0, 0, 0, 0 };
// Alpha background (subtle)
inline constexpr Color SageA2{ 240, 242, 241, 8 };
// Alpha interactive background
inline constexpr Color SageA3{ 243, 245, 244, 18 };
// Alpha interactive background (hover)
inline constexpr Color SageA4{ 242, 254, 253, 26 };
// Alpha interactive background (active)
inline constexpr Color SageA5{ 241, 251, 250, 34 };
// Alpha border (subtle)
inline constexpr Color SageA6{ 237, 251, 244, 45 };
// Alpha border
inline constexpr Color SageA7{ 237, 252, 247, 60 };
// Alpha border (strong)
inline constexpr Color SageA8{ 235, 253, 246, 87 };
// Alpha solid
inline constexpr Color SageA9{ 223, 253, 242, 102 };
// Alpha solid (hover)
inline constexpr Color SageA10{ 229, 253, 246, 116 };
// Alpha text
inline constexpr Color SageA11{ 244, 254, 251, 176 };
// Alpha text (high contrast)
inline constexpr Color SageA12{ 253, 255, 254, 237 };

// Background
inline constexpr Color Olive1{ 17, 18, 16, 255 };
// Background (subtle)
inline constexpr Color Olive2{ 24, 25, 23, 255 };
// Interactive background
inline constexpr Color Olive3{ 33, 34, 32, 255 };
// Interactive background (hover)
inline constexpr Color Olive4{ 40, 42, 39, 255 };
// Interactive background (active)
inline constexpr Color Olive5{ 47, 49, 46, 255 };
// Border (subtle)
inline constexpr Color Olive6{ 56, 58, 54, 255 };
// Border
inline constexpr Color Olive7{ 69, 72, 67, 255 };
// Border (strong)
inline constexpr Color Olive8{ 92, 98, 91, 255 };
// Solid
inline constexpr Color Olive9{ 104, 112, 102, 255 };
// Solid (hover)
inline constexpr Color Olive10{ 118, 125, 116, 255 };
// Text
inline constexpr Color Olive11{ 175, 181, 173, 255 };
// Text (high contrast)
inline constexpr Color Olive12{ 236, 238, 236, 255 };

// Alpha background
inline constexpr Color OliveA1{ 0, 0, 0, 0 };
// Alpha background (subtle)
inline constexpr Color OliveA2{ 241, 242, 240, 8 };
// Alpha interactive background
inline constexpr Color OliveA3{ 244, 245, 243, 18 };
// Alpha interactive background (hover)
inline constexpr Color OliveA4{ 243, 254, 242, 26 };
// Alpha interactive background (active)
inline constexpr Color OliveA5{ 242, 251, 241, 34 };
// Alpha border (subtle)
inline constexpr Color OliveA6{ 244, 250, 237, 44 };
// Alpha border
inline constexpr Color OliveA7{ 242, 252, 237, 59 };
// Alpha border (strong)
inline constexpr Color OliveA8{ 237, 253, 235, 87 };
// Alpha solid
inline constexpr Color OliveA9{ 235, 253, 231, 102 };
// Alpha solid (hover)
inline constexpr Color OliveA10{ 240, 253, 236, 116 };
// Alpha text
inline constexpr Color OliveA11{ 246, 254, 244, 176 };
// Alpha text (high contrast)
inline constexpr Color OliveA12{ 253, 255, 253, 237 };

// Background
inline constexpr Color Sand1{ 17, 17, 16, 255 };
// Background (subtle)
inline constexpr Color Sand2{ 25, 25, 24, 255 };
// Interactive background
inline constexpr Color Sand3{ 34, 34, 33, 255 };
// Interactive background (hover)
inline constexpr Color Sand4{ 42, 42, 40, 255 };
// Interactive background (active)
inline constexpr Color Sand5{ 49, 49, 46, 255 };
// Border (subtle)
inline constexpr Color Sand6{ 59, 58, 55, 255 };
// Border
inline constexpr Color Sand7{ 73, 72, 68, 255 };
// Border (strong)
inline constexpr Color Sand8{ 98, 96, 91, 255 };
// Solid
inline constexpr Color Sand9{ 111, 109, 102, 255 };
// Solid (hover)
inline constexpr Color Sand10{ 124, 123, 116, 255 };
// Text
inline constexpr Color Sand11{ 181, 179, 173, 255 };
// Text (high contrast)
inline constexpr Color Sand12{ 238, 238, 236, 255 };

// Alpha background
inline constexpr Color SandA1{ 0, 0, 0, 0 };
// Alpha background (subtle)
inline constexpr Color SandA2{ 244, 244, 243, 9 };
// Alpha interactive background
inline constexpr Color SandA3{ 246, 246, 245, 19 };
// Alpha interactive background (hover)
inline constexpr Color SandA4{ 254, 254, 243, 27 };
// Alpha interactive background (active)
inline constexpr Color SandA5{ 251, 251, 235, 35 };
// Alpha border (subtle)
inline constexpr Color SandA6{ 255, 250, 237, 45 };
// Alpha border
inline constexpr Color SandA7{ 255, 251, 237, 60 };
// Alpha border (strong)
inline constexpr Color SandA8{ 255, 249, 235, 87 };
// Alpha solid
inline constexpr Color SandA9{ 255, 250, 233, 101 };
// Alpha solid (hover)
inline constexpr Color SandA10{ 255, 253, 238, 115 };
// Alpha text
inline constexpr Color SandA11{ 255, 252, 244, 176 };
// Alpha text (high contrast)
inline constexpr Color SandA12{ 255, 255, 253, 237 };

// Background
inline constexpr Color Tomato1{ 24, 17, 17, 255 };
// Background (subtle)
inline constexpr Color Tomato2{ 31, 21, 19, 255 };
// Interactive background
inline constexpr Color Tomato3{ 57, 23, 20, 255 };
// Interactive background (hover)
inline constexpr Color Tomato4{ 78, 21, 17, 255 };
// Interactive background (active)
inline constexpr Color Tomato5{ 94, 28, 22, 255 };
// Border (subtle)
inline constexpr Color Tomato6{ 110, 41, 32, 255 };
// Border
inline constexpr Color Tomato7{ 133, 58, 45, 255 };
// Border (strong)
inline constexpr Color Tomato8{ 172, 77, 57, 255 };
// Solid
inline constexpr Color Tomato9{ 229, 77, 46, 255 };
// Solid (hover)
inline constexpr Color Tomato10{ 236, 97, 66, 255 };
// Text
inline constexpr Color Tomato11{ 255, 151, 125, 255 };
// Text (high contrast)
inline constexpr Color Tomato12{ 251, 211, 203, 255 };

// Alpha background
inline constexpr Color TomatoA1{ 241, 18, 18, 8 };
// Alpha background (subtle)
inline constexpr Color TomatoA2{ 255, 85, 51, 15 };
// Alpha interactive background
inline constexpr Color TomatoA3{ 255, 53, 35, 43 };
// Alpha interactive background (hover)
inline constexpr Color TomatoA4{ 253, 32, 17, 66 };
// Alpha interactive background (active)
inline constexpr Color TomatoA5{ 254, 51, 33, 83 };
// Alpha border (subtle)
inline constexpr Color TomatoA6{ 255, 79, 56, 100 };
// Alpha border
inline constexpr Color TomatoA7{ 253, 100, 74, 125 };
// Alpha border (strong)
inline constexpr Color TomatoA8{ 254, 109, 78, 167 };
// Alpha solid
inline constexpr Color TomatoA9{ 254, 84, 49, 228 };
// Alpha solid (hover)
inline constexpr Color TomatoA10{ 255, 104, 71, 235 };
// Alpha text
inline constexpr Color TomatoA11{ 255, 151, 125, 255 };
// Alpha text (high contrast)
inline constexpr Color TomatoA12{ 255, 214, 206, 251 };

// Background
inline constexpr Color Red1{ 25, 17, 17, 255 };
// Background (subtle)
inline constexpr Color Red2{ 32, 19, 20, 255 };
// Interactive background
inline constexpr Color Red3{ 59, 18, 25, 255 };
// Interactive background (hover)
inline constexpr Color Red4{ 80, 15, 28, 255 };
// Interactive background (active)
inline constexpr Color Red5{ 97, 22, 35, 255 };
// Border (subtle)
inline constexpr Color Red6{ 114, 35, 45, 255 };
// Border
inline constexpr Color Red7{ 140, 51, 58, 255 };
// Border (strong)
inline constexpr Color Red8{ 181, 69, 72, 255 };
// Solid
inline constexpr Color Red9{ 229, 72, 77, 255 };
// Solid (hover)
inline constexpr Color Red10{ 236, 93, 94, 255 };
// Text
inline constexpr Color Red11{ 255, 149, 146, 255 };
// Text (high contrast)
inline constexpr Color Red12{ 255, 209, 217, 255 };

// Alpha background
inline constexpr Color RedA1{ 244, 18, 18, 9 };
// Alpha background (subtle)
inline constexpr Color RedA2{ 242, 47, 62, 17 };
// Alpha interactive background
inline constexpr Color RedA3{ 255, 23, 63, 45 };
// Alpha interactive background (hover)
inline constexpr Color RedA4{ 254, 10, 59, 68 };
// Alpha interactive background (active)
inline constexpr Color RedA5{ 255, 32, 71, 86 };
// Alpha border (subtle)
inline constexpr Color RedA6{ 255, 62, 86, 104 };
// Alpha border
inline constexpr Color RedA7{ 255, 83, 97, 132 };
// Alpha border (strong)
inline constexpr Color RedA8{ 255, 93, 97, 176 };
// Alpha solid
inline constexpr Color RedA9{ 254, 78, 84, 228 };
// Alpha solid (hover)
inline constexpr Color RedA10{ 255, 100, 101, 235 };
// Alpha text
inline constexpr Color RedA11{ 255, 149, 146, 255 };
// Alpha text (high contrast)
inline constexpr Color RedA12{ 255, 209, 217, 255 };

// Background
inline constexpr Color Ruby1{ 25, 17, 19, 255 };
// Background (subtle)
inline constexpr Color Ruby2{ 30, 21, 23, 255 };
// Interactive background
inline constexpr Color Ruby3{ 58, 20, 30, 255 };
// Interactive background (hover)
inline constexpr Color Ruby4{ 78, 19, 37, 255 };
// Interactive background (active)
inline constexpr Color Ruby5{ 94, 26, 46, 255 };
// Border (subtle)
inline constexpr Color Ruby6{ 111, 37, 57, 255 };
// Border
inline constexpr Color Ruby7{ 136, 52, 71, 255 };
// Border (strong)
inline constexpr Color Ruby8{ 179, 68, 90, 255 };
// Solid
inline constexpr Color Ruby9{ 229, 70, 102, 255 };
// Solid (hover)
inline constexpr Color Ruby10{ 236, 90, 114, 255 };
// Text
inline constexpr Color Ruby11{ 255, 148, 157, 255 };
// Text (high contrast)
inline constexpr Color Ruby12{ 254, 210, 225, 255 };

// Alpha background
inline constexpr Color RubyA1{ 244, 18, 74, 9 };
// Alpha background (subtle)
inline constexpr Color RubyA2{ 254, 90, 127, 14 };
// Alpha interactive background
inline constexpr Color RubyA3{ 255, 35, 93, 44 };
// Alpha interactive background (hover)
inline constexpr Color RubyA4{ 253, 25, 94, 66 };
// Alpha interactive background (active)
inline constexpr Color RubyA5{ 254, 45, 107, 83 };
// Alpha border (subtle)
inline constexpr Color RubyA6{ 255, 68, 118, 101 };
// Alpha border
inline constexpr Color RubyA7{ 255, 87, 125, 128 };
// Alpha border (strong)
inline constexpr Color RubyA8{ 255, 92, 124, 174 };
// Alpha solid
inline constexpr Color RubyA9{ 254, 76, 112, 228 };
// Alpha solid (hover)
inline constexpr Color RubyA10{ 255, 97, 123, 235 };
// Alpha text
inline constexpr Color RubyA11{ 255, 148, 157, 255 };
// Alpha text (high contrast)
inline constexpr Color RubyA12{ 255, 211, 226, 254 };

// Background
inline constexpr Color Crimson1{ 25, 17, 20, 255 };
// Background (subtle)
inline constexpr Color Crimson2{ 32, 19, 24, 255 };
// Interactive background
inline constexpr Color Crimson3{ 56, 21, 37, 255 };
// Interactive background (hover)
inline constexpr Color Crimson4{ 77, 18, 47, 255 };
// Interactive background (active)
inline constexpr Color Crimson5{ 92, 24, 57, 255 };
// Border (subtle)
inline constexpr Color Crimson6{ 109, 37, 69, 255 };
// Border
inline constexpr Color Crimson7{ 135, 51, 86, 255 };
// Border (strong)
inline constexpr Color Crimson8{ 176, 67, 110, 255 };
// Solid
inline constexpr Color Crimson9{ 233, 61, 130, 255 };
// Solid (hover)
inline constexpr Color Crimson10{ 238, 81, 138, 255 };
// Text
inline constexpr Color Crimson11{ 255, 146, 173, 255 };
// Text (high contrast)
inline constexpr Color Crimson12{ 253, 211, 232, 255 };

// Alpha background
inline constexpr Color CrimsonA1{ 244, 18, 103, 9 };
// Alpha background (subtle)
inline constexpr Color CrimsonA2{ 242, 47, 122, 17 };
// Alpha interactive background
inline constexpr Color CrimsonA3{ 254, 42, 139, 42 };
// Alpha interactive background (hover)
inline constexpr Color CrimsonA4{ 253, 21, 135, 65 };
// Alpha interactive background (active)
inline constexpr Color CrimsonA5{ 253, 39, 143, 81 };
// Alpha border (subtle)
inline constexpr Color CrimsonA6{ 254, 69, 151, 99 };
// Alpha border
inline constexpr Color CrimsonA7{ 253, 85, 155, 127 };
// Alpha border (strong)
inline constexpr Color CrimsonA8{ 254, 91, 155, 171 };
// Alpha solid
inline constexpr Color CrimsonA9{ 254, 65, 141, 232 };
// Alpha solid (hover)
inline constexpr Color CrimsonA10{ 255, 86, 147, 237 };
// Alpha text
inline constexpr Color CrimsonA11{ 255, 146, 173, 255 };
// Alpha text (high contrast)
inline constexpr Color CrimsonA12{ 255, 213, 234, 253 };

// Background
inline constexpr Color Pink1{ 25, 17, 23, 255 };
// Background (subtle)
inline constexpr Color Pink2{ 33, 18, 29, 255 };
// Interactive background
inline constexpr Color Pink3{ 55, 23, 47, 255 };
// Interactive background (hover)
inline constexpr Color Pink4{ 75, 20, 61, 255 };
// Interactive background (active)
inline constexpr Color Pink5{ 89, 28, 71, 255 };
// Border (subtle)
inline constexpr Color Pink6{ 105, 41, 85, 255 };
// Border
inline constexpr Color Pink7{ 131, 56, 105, 255 };
// Border (strong)
inline constexpr Color Pink8{ 168, 72, 133, 255 };
// Solid
inline constexpr Color Pink9{ 214, 64, 159, 255 };
// Solid (hover)
inline constexpr Color Pink10{ 222, 81, 168, 255 };
// Text
inline constexpr Color Pink11{ 255, 141, 204, 255 };
// Text (high contrast)
inline constexpr Color Pink12{ 253, 209, 234, 255 };

// Alpha background
inline constexpr Color PinkA1{ 244, 18, 188, 9 };
// Alpha background (subtle)
inline constexpr Color PinkA2{ 244, 32, 187, 18 };
// Alpha interactive background
inline constexpr Color PinkA3{ 254, 55, 204, 41 };
// Alpha interactive background (hover)
inline constexpr Color PinkA4{ 252, 30, 196, 63 };
// Alpha interactive background (active)
inline constexpr Color PinkA5{ 253, 53, 194, 78 };
// Alpha border (subtle)
inline constexpr Color PinkA6{ 253, 81, 199, 95 };
// Alpha border
inline constexpr Color PinkA7{ 253, 98, 200, 123 };
// Alpha border (strong)
inline constexpr Color PinkA8{ 255, 104, 200, 162 };
// Alpha solid
inline constexpr Color PinkA9{ 254, 73, 188, 212 };
// Alpha solid (hover)
inline constexpr Color PinkA10{ 255, 92, 192, 220 };
// Alpha text
inline constexpr Color PinkA11{ 255, 141, 204, 255 };
// Alpha text (high contrast)
inline constexpr Color PinkA12{ 255, 211, 236, 253 };

// Background
inline constexpr Color Plum1{ 24, 17, 24, 255 };
// Background (subtle)
inline constexpr Color Plum2{ 32, 19, 32, 255 };
// Interactive background
inline constexpr Color Plum3{ 53, 26, 53, 255 };
// Interactive background (hover)
inline constexpr Color Plum4{ 69, 29, 71, 255 };
// Interactive background (active)
inline constexpr Color Plum5{ 81, 36, 84, 255 };
// Border (subtle)
inline constexpr Color Plum6{ 94, 48, 97, 255 };
// Border
inline constexpr Color Plum7{ 115, 64, 121, 255 };
// Border (strong)
inline constexpr Color Plum8{ 146, 84, 156, 255 };
// Solid
inline constexpr Color Plum9{ 171, 74, 186, 255 };
// Solid (hover)
inline constexpr Color Plum10{ 182, 88, 196, 255 };
// Text
inline constexpr Color Plum11{ 231, 150, 243, 255 };
// Text (high contrast)
inline constexpr Color Plum12{ 244, 212, 244, 255 };

// Alpha background
inline constexpr Color PlumA1{ 241, 18, 241, 8 };
// Alpha background (subtle)
inline constexpr Color PlumA2{ 242, 47, 242, 17 };
// Alpha interactive background
inline constexpr Color PlumA3{ 253, 76, 253, 39 };
// Alpha interactive background (hover)
inline constexpr Color PlumA4{ 246, 70, 255, 58 };
// Alpha interactive background (active)
inline constexpr Color PlumA5{ 244, 85, 255, 72 };
// Alpha border (subtle)
inline constexpr Color PlumA6{ 246, 109, 255, 86 };
// Alpha border
inline constexpr Color PlumA7{ 240, 124, 253, 112 };
// Alpha border (strong)
inline constexpr Color PlumA8{ 238, 132, 255, 149 };
// Alpha solid
inline constexpr Color PlumA9{ 233, 97, 254, 182 };
// Alpha solid (hover)
inline constexpr Color PlumA10{ 237, 112, 255, 192 };
// Alpha text
inline constexpr Color PlumA11{ 241, 156, 254, 243 };
// Alpha text (high contrast)
inline constexpr Color PlumA12{ 254, 221, 254, 244 };

// Background
inline constexpr Color Purple1{ 24, 17, 27, 255 };
// Background (subtle)
inline constexpr Color Purple2{ 30, 21, 35, 255 };
// Interactive background
inline constexpr Color Purple3{ 48, 28, 59, 255 };
// Interactive background (hover)
inline constexpr Color Purple4{ 61, 34, 78, 255 };
// Interactive background (active)
inline constexpr Color Purple5{ 72, 41, 92, 255 };
// Border (subtle)
inline constexpr Color Purple6{ 84, 52, 107, 255 };
// Border
inline constexpr Color Purple7{ 102, 66, 130, 255 };
// Border (strong)
inline constexpr Color Purple8{ 132, 87, 170, 255 };
// Solid
inline constexpr Color Purple9{ 142, 78, 198, 255 };
// Solid (hover)
inline constexpr Color Purple10{ 154, 92, 208, 255 };
// Text
inline constexpr Color Purple11{ 209, 157, 255, 255 };
// Text (high contrast)
inline constexpr Color Purple12{ 236, 217, 250, 255 };

// Alpha background
inline constexpr Color PurpleA1{ 180, 18, 249, 11 };
// Alpha background (subtle)
inline constexpr Color PurpleA2{ 183, 68, 247, 20 };
// Alpha interactive background
inline constexpr Color PurpleA3{ 193, 80, 255, 45 };
// Alpha interactive background (hover)
inline constexpr Color PurpleA4{ 187, 83, 253, 66 };
// Alpha interactive background (active)
inline constexpr Color PurpleA5{ 190, 92, 253, 81 };
// Alpha border (subtle)
inline constexpr Color PurpleA6{ 193, 109, 253, 97 };
// Alpha border
inline constexpr Color PurpleA7{ 195, 120, 253, 122 };
// Alpha border (strong)
inline constexpr Color PurpleA8{ 196, 126, 255, 164 };
// Alpha solid
inline constexpr Color PurpleA9{ 182, 97, 255, 194 };
// Alpha solid (hover)
inline constexpr Color PurpleA10{ 188, 111, 255, 205 };
// Alpha text
inline constexpr Color PurpleA11{ 209, 157, 255, 255 };
// Alpha text (high contrast)
inline constexpr Color PurpleA12{ 241, 221, 255, 250 };

// Background
inline constexpr Color Violet1{ 20, 18, 31, 255 };
// Background (subtle)
inline constexpr Color Violet2{ 27, 21, 37, 255 };
// Interactive background
inline constexpr Color Violet3{ 41, 31, 67, 255 };
// Interactive background (hover)
inline constexpr Color Violet4{ 51, 37, 91, 255 };
// Interactive background (active)
inline constexpr Color Violet5{ 60, 46, 105, 255 };
// Border (subtle)
inline constexpr Color Violet6{ 71, 56, 118, 255 };
// Border
inline constexpr Color Violet7{ 86, 70, 139, 255 };
// Border (strong)
inline constexpr Color Violet8{ 105, 88, 173, 255 };
// Solid
inline constexpr Color Violet9{ 110, 86, 207, 255 };
// Solid (hover)
inline constexpr Color Violet10{ 125, 102, 217, 255 };
// Text
inline constexpr Color Violet11{ 186, 167, 255, 255 };
// Text (high contrast)
inline constexpr Color Violet12{ 226, 221, 254, 255 };

// Alpha background
inline constexpr Color VioletA1{ 68, 34, 255, 15 };
// Alpha background (subtle)
inline constexpr Color VioletA2{ 133, 63, 249, 22 };
// Alpha interactive background
inline constexpr Color VioletA3{ 131, 84, 254, 54 };
// Alpha interactive background (hover)
inline constexpr Color VioletA4{ 125, 81, 253, 80 };
// Alpha interactive background (active)
inline constexpr Color VioletA5{ 132, 95, 253, 95 };
// Alpha border (subtle)
inline constexpr Color VioletA6{ 143, 108, 253, 109 };
// Alpha border
inline constexpr Color VioletA7{ 152, 121, 255, 131 };
// Alpha border (strong)
inline constexpr Color VioletA8{ 151, 125, 254, 168 };
// Alpha solid
inline constexpr Color VioletA9{ 134, 104, 255, 204 };
// Alpha solid (hover)
inline constexpr Color VioletA10{ 145, 118, 254, 215 };
// Alpha text
inline constexpr Color VioletA11{ 186, 167, 255, 255 };
// Alpha text (high contrast)
inline constexpr Color VioletA12{ 227, 222, 255, 254 };

// Background
inline constexpr Color Iris1{ 19, 19, 30, 255 };
// Background (subtle)
inline constexpr Color Iris2{ 23, 22, 37, 255 };
// Interactive background
inline constexpr Color Iris3{ 32, 34, 72, 255 };
// Interactive background (hover)
inline constexpr Color Iris4{ 38, 42, 101, 255 };
// Interactive background (active)
inline constexpr Color Iris5{ 48, 51, 116, 255 };
// Border (subtle)
inline constexpr Color Iris6{ 61, 62, 130, 255 };
// Border
inline constexpr Color Iris7{ 74, 74, 149, 255 };
// Border (strong)
inline constexpr Color Iris8{ 89, 88, 177, 255 };
// Solid
inline constexpr Color Iris9{ 91, 91, 214, 255 };
// Solid (hover)
inline constexpr Color Iris10{ 110, 106, 222, 255 };
// Text
inline constexpr Color Iris11{ 177, 169, 255, 255 };
// Text (high contrast)
inline constexpr Color Iris12{ 224, 223, 254, 255 };

// Alpha background
inline constexpr Color IrisA1{ 54, 54, 254, 14 };
// Alpha background (subtle)
inline constexpr Color IrisA2{ 86, 75, 249, 22 };
// Alpha interactive background
inline constexpr Color IrisA3{ 82, 91, 255, 59 };
// Alpha interactive background (hover)
inline constexpr Color IrisA4{ 77, 88, 255, 90 };
// Alpha interactive background (active)
inline constexpr Color IrisA5{ 91, 98, 253, 107 };
// Alpha border (subtle)
inline constexpr Color IrisA6{ 109, 111, 253, 122 };
// Alpha border
inline constexpr Color IrisA7{ 119, 119, 254, 142 };
// Alpha border (strong)
inline constexpr Color IrisA8{ 123, 122, 254, 172 };
// Alpha solid
inline constexpr Color IrisA9{ 106, 106, 254, 212 };
// Alpha solid (hover)
inline constexpr Color IrisA10{ 125, 121, 255, 220 };
// Alpha text
inline constexpr Color IrisA11{ 177, 169, 255, 255 };
// Alpha text (high contrast)
inline constexpr Color IrisA12{ 225, 224, 255, 254 };

// Background
inline constexpr Color Indigo1{ 17, 19, 31, 255 };
// Background (subtle)
inline constexpr Color Indigo2{ 20, 23, 38, 255 };
// Interactive background
inline constexpr Color Indigo3{ 24, 36, 73, 255 };
// Interactive background (hover)
inline constexpr Color Indigo4{ 29, 46, 98, 255 };
// Interactive background (active)
inline constexpr Color Indigo5{ 37, 57, 116, 255 };
// Border (subtle)
inline constexpr Color Indigo6{ 48, 67, 132, 255 };
// Border
inline constexpr Color Indigo7{ 58, 79, 151, 255 };
// Border (strong)
inline constexpr Color Indigo8{ 67, 93, 177, 255 };
// Solid
inline constexpr Color Indigo9{ 62, 99, 221, 255 };
// Solid (hover)
inline constexpr Color Indigo10{ 84, 114, 228, 255 };
// Text
inline constexpr Color Indigo11{ 158, 177, 255, 255 };
// Text (high contrast)
inline constexpr Color Indigo12{ 214, 225, 255, 255 };

// Alpha background
inline constexpr Color IndigoA1{ 17, 51, 255, 15 };
// Alpha background (subtle)
inline constexpr Color IndigoA2{ 51, 84, 250, 23 };
// Alpha interactive background
inline constexpr Color IndigoA3{ 47, 98, 255, 60 };
// Alpha interactive background (hover)
inline constexpr Color IndigoA4{ 53, 102, 255, 87 };
// Alpha interactive background (active)
inline constexpr Color IndigoA5{ 65, 113, 253, 107 };
// Alpha border (subtle)
inline constexpr Color IndigoA6{ 81, 120, 253, 124 };
// Alpha border
inline constexpr Color IndigoA7{ 90, 127, 255, 144 };
// Alpha border (strong)
inline constexpr Color IndigoA8{ 91, 129, 254, 172 };
// Alpha solid
inline constexpr Color IndigoA9{ 70, 113, 255, 219 };
// Alpha solid (hover)
inline constexpr Color IndigoA10{ 92, 126, 254, 227 };
// Alpha text
inline constexpr Color IndigoA11{ 158, 177, 255, 255 };
// Alpha text (high contrast)
inline constexpr Color IndigoA12{ 214, 225, 255, 255 };

// Background
inline constexpr Color Blue1{ 13, 21, 32, 255 };
// Background (subtle)
inline constexpr Color Blue2{ 17, 25, 39, 255 };
// Interactive background
inline constexpr Color Blue3{ 13, 40, 71, 255 };
// Interactive background (hover)
inline constexpr Color Blue4{ 0, 51, 98, 255 };
// Interactive background (active)
inline constexpr Color Blue5{ 0, 64, 116, 255 };
// Border (subtle)
inline constexpr Color Blue6{ 16, 77, 135, 255 };
// Border
inline constexpr Color Blue7{ 32, 93, 158, 255 };
// Border (strong)
inline constexpr Color Blue8{ 40, 112, 189, 255 };
// Solid
inline constexpr Color Blue9{ 0, 144, 255, 255 };
// Solid (hover)
inline constexpr Color Blue10{ 59, 158, 255, 255 };
// Text
inline constexpr Color Blue11{ 112, 184, 255, 255 };
// Text (high contrast)
inline constexpr Color Blue12{ 194, 230, 255, 255 };

// Alpha background
inline constexpr Color BlueA1{ 0, 77, 242, 17 };
// Alpha background (subtle)
inline constexpr Color BlueA2{ 17, 102, 251, 24 };
// Alpha interactive background
inline constexpr Color BlueA3{ 0, 119, 255, 58 };
// Alpha interactive background (hover)
inline constexpr Color BlueA4{ 0, 117, 255, 87 };
// Alpha interactive background (active)
inline constexpr Color BlueA5{ 0, 129, 253, 107 };
// Alpha border (subtle)
inline constexpr Color BlueA6{ 15, 137, 253, 127 };
// Alpha border
inline constexpr Color BlueA7{ 42, 145, 254, 152 };
// Alpha border (strong)
inline constexpr Color BlueA8{ 48, 148, 254, 185 };
// Alpha solid
inline constexpr Color BlueA9{ 0, 144, 255, 255 };
// Alpha solid (hover)
inline constexpr Color BlueA10{ 59, 158, 255, 255 };
// Alpha text
inline constexpr Color BlueA11{ 112, 184, 255, 255 };
// Alpha text (high contrast)
inline constexpr Color BlueA12{ 194, 230, 255, 255 };

// Background
inline constexpr Color Cyan1{ 11, 22, 26, 255 };
// Background (subtle)
inline constexpr Color Cyan2{ 16, 27, 32, 255 };
// Interactive background
inline constexpr Color Cyan3{ 8, 44, 54, 255 };
// Interactive background (hover)
inline constexpr Color Cyan4{ 0, 56, 72, 255 };
// Interactive background (active)
inline constexpr Color Cyan5{ 0, 69, 88, 255 };
// Border (subtle)
inline constexpr Color Cyan6{ 4, 84, 104, 255 };
// Border
inline constexpr Color Cyan7{ 18, 103, 126, 255 };
// Border (strong)
inline constexpr Color Cyan8{ 17, 128, 156, 255 };
// Solid
inline constexpr Color Cyan9{ 0, 162, 199, 255 };
// Solid (hover)
inline constexpr Color Cyan10{ 35, 175, 208, 255 };
// Text
inline constexpr Color Cyan11{ 76, 204, 230, 255 };
// Text (high contrast)
inline constexpr Color Cyan12{ 182, 236, 247, 255 };

// Alpha background
inline constexpr Color CyanA1{ 0, 145, 247, 10 };
// Alpha background (subtle)
inline constexpr Color CyanA2{ 2, 167, 242, 17 };
// Alpha interactive background
inline constexpr Color CyanA3{ 0, 190, 253, 40 };
// Alpha interactive background (hover)
inline constexpr Color CyanA4{ 0, 186, 255, 59 };
// Alpha interactive background (active)
inline constexpr Color CyanA5{ 0, 190, 253, 77 };
// Alpha border (subtle)
inline constexpr Color CyanA6{ 0, 199, 253, 94 };
// Alpha border
inline constexpr Color CyanA7{ 20, 205, 255, 117 };
// Alpha border (strong)
inline constexpr Color CyanA8{ 17, 207, 255, 149 };
// Alpha solid
inline constexpr Color CyanA9{ 0, 207, 255, 195 };
// Alpha solid (hover)
inline constexpr Color CyanA10{ 40, 214, 255, 205 };
// Alpha text
inline constexpr Color CyanA11{ 82, 225, 254, 229 };
// Alpha text (high contrast)
inline constexpr Color CyanA12{ 187, 243, 254, 247 };

// Background
inline constexpr Color Sky1{ 13, 20, 31, 255 };
// Background (subtle)
inline constexpr Color Sky2{ 17, 26, 39, 255 };
// Interactive background
inline constexpr Color Sky3{ 17, 40, 64, 255 };
// Interactive background (hover)
inline constexpr Color Sky4{ 17, 53, 85, 255 };
// Interactive background (active)
inline constexpr Color Sky5{ 21, 68, 103, 255 };
// Border (subtle)
inline constexpr Color Sky6{ 27, 83, 123, 255 };
// Border
inline constexpr Color Sky7{ 31, 102, 146, 255 };
// Border (strong)
inline constexpr Color Sky8{ 25, 124, 174, 255 };
// Solid
inline constexpr Color Sky9{ 124, 226, 254, 255 };
// Solid (hover)
inline constexpr Color Sky10{ 168, 238, 255, 255 };
// Text
inline constexpr Color Sky11{ 117, 199, 240, 255 };
// Text (high contrast)
inline constexpr Color Sky12{ 194, 243, 255, 255 };

// Alpha background
inline constexpr Color SkyA1{ 0, 68, 255, 15 };
// Alpha background (subtle)
inline constexpr Color SkyA2{ 17, 113, 251, 24 };
// Alpha interactive background
inline constexpr Color SkyA3{ 17, 132, 252, 51 };
// Alpha interactive background (hover)
inline constexpr Color SkyA4{ 18, 143, 255, 73 };
// Alpha interactive background (active)
inline constexpr Color SkyA5{ 28, 157, 253, 93 };
// Alpha border (subtle)
inline constexpr Color SkyA6{ 40, 165, 255, 114 };
// Alpha border
inline constexpr Color SkyA7{ 43, 173, 254, 139 };
// Alpha border (strong)
inline constexpr Color SkyA8{ 29, 178, 254, 169 };
// Alpha solid
inline constexpr Color SkyA9{ 124, 227, 255, 254 };
// Alpha solid (hover)
inline constexpr Color SkyA10{ 168, 238, 255, 255 };
// Alpha text
inline constexpr Color SkyA11{ 124, 211, 255, 239 };
// Alpha text (high contrast)
inline constexpr Color SkyA12{ 194, 243, 255, 255 };

// Background
inline constexpr Color Teal1{ 13, 21, 20, 255 };
// Background (subtle)
inline constexpr Color Teal2{ 17, 28, 27, 255 };
// Interactive background
inline constexpr Color Teal3{ 13, 45, 42, 255 };
// Interactive background (hover)
inline constexpr Color Teal4{ 2, 59, 55, 255 };
// Interactive background (active)
inline constexpr Color Teal5{ 8, 72, 67, 255 };
// Border (subtle)
inline constexpr Color Teal6{ 20, 87, 80, 255 };
// Border
inline constexpr Color Teal7{ 28, 105, 97, 255 };
// Border (strong)
inline constexpr Color Teal8{ 32, 126, 115, 255 };
// Solid
inline constexpr Color Teal9{ 18, 165, 148, 255 };
// Solid (hover)
inline constexpr Color Teal10{ 14, 179, 158, 255 };
// Text
inline constexpr Color Teal11{ 11, 216, 182, 255 };
// Text (high contrast)
inline constexpr Color Teal12{ 173, 240, 221, 255 };

// Alpha background
inline constexpr Color TealA1{ 0, 222, 171, 5 };
// Alpha background (subtle)
inline constexpr Color TealA2{ 18, 251, 230, 12 };
// Alpha interactive background
inline constexpr Color TealA3{ 0, 255, 230, 30 };
// Alpha interactive background (hover)
inline constexpr Color TealA4{ 0, 255, 233, 45 };
// Alpha interactive background (active)
inline constexpr Color TealA5{ 0, 255, 234, 59 };
// Alpha border (subtle)
inline constexpr Color TealA6{ 28, 255, 232, 75 };
// Alpha border
inline constexpr Color TealA7{ 46, 253, 232, 95 };
// Alpha border (strong)
inline constexpr Color TealA8{ 50, 255, 231, 117 };
// Alpha solid
inline constexpr Color TealA9{ 19, 255, 228, 159 };
// Alpha solid (hover)
inline constexpr Color TealA10{ 13, 255, 224, 174 };
// Alpha text
inline constexpr Color TealA11{ 10, 254, 213, 214 };
// Alpha text (high contrast)
inline constexpr Color TealA12{ 184, 255, 235, 239 };

// Background
inline constexpr Color Jade1{ 13, 21, 18, 255 };
// Background (subtle)
inline constexpr Color Jade2{ 18, 28, 24, 255 };
// Interactive background
inline constexpr Color Jade3{ 15, 46, 34, 255 };
// Interactive background (hover)
inline constexpr Color Jade4{ 11, 59, 44, 255 };
// Interactive background (active)
inline constexpr Color Jade5{ 17, 72, 55, 255 };
// Border (subtle)
inline constexpr Color Jade6{ 27, 87, 69, 255 };
// Border
inline constexpr Color Jade7{ 36, 104, 84, 255 };
// Border (strong)
inline constexpr Color Jade8{ 42, 126, 104, 255 };
// Solid
inline constexpr Color Jade9{ 41, 163, 131, 255 };
// Solid (hover)
inline constexpr Color Jade10{ 39, 176, 139, 255 };
// Text
inline constexpr Color Jade11{ 31, 216, 164, 255 };
// Text (high contrast)
inline constexpr Color Jade12{ 173, 240, 212, 255 };

// Alpha background
inline constexpr Color JadeA1{ 0, 222, 69, 5 };
// Alpha background (subtle)
inline constexpr Color JadeA2{ 39, 251, 166, 12 };
// Alpha interactive background
inline constexpr Color JadeA3{ 2, 249, 153, 32 };
// Alpha interactive background (hover)
inline constexpr Color JadeA4{ 0, 255, 170, 45 };
// Alpha interactive background (active)
inline constexpr Color JadeA5{ 17, 255, 182, 59 };
// Alpha border (subtle)
inline constexpr Color JadeA6{ 52, 255, 194, 75 };
// Alpha border
inline constexpr Color JadeA7{ 69, 253, 199, 94 };
// Alpha border (strong)
inline constexpr Color JadeA8{ 72, 255, 207, 117 };
// Alpha solid
inline constexpr Color JadeA9{ 56, 254, 202, 157 };
// Alpha solid (hover)
inline constexpr Color JadeA10{ 49, 254, 199, 171 };
// Alpha text
inline constexpr Color JadeA11{ 33, 254, 192, 214 };
// Alpha text (high contrast)
inline constexpr Color JadeA12{ 184, 255, 225, 239 };

// Background
inline constexpr Color Green1{ 14, 21, 18, 255 };
// Background (subtle)
inline constexpr Color Green2{ 18, 27, 23, 255 };
// Interactive background
inline constexpr Color Green3{ 19, 45, 33, 255 };
// Interactive background (hover)
inline constexpr Color Green4{ 17, 59, 41, 255 };
// Interactive background (active)
inline constexpr Color Green5{ 23, 73, 51, 255 };
// Border (subtle)
inline constexpr Color Green6{ 32, 87, 62, 255 };
// Border
inline constexpr Color Green7{ 40, 104, 74, 255 };
// Border (strong)
inline constexpr Color Green8{ 47, 124, 87, 255 };
// Solid
inline constexpr Color Green9{ 48, 164, 108, 255 };
// Solid (hover)
inline constexpr Color Green10{ 51, 176, 116, 255 };
// Text
inline constexpr Color Green11{ 61, 214, 140, 255 };
// Text (high contrast)
inline constexpr Color Green12{ 177, 241, 203, 255 };

// Alpha background
inline constexpr Color GreenA1{ 0, 222, 69, 5 };
// Alpha background (subtle)
inline constexpr Color GreenA2{ 41, 249, 157, 11 };
// Alpha interactive background
inline constexpr Color GreenA3{ 34, 255, 153, 30 };
// Alpha interactive background (hover)
inline constexpr Color GreenA4{ 17, 255, 153, 45 };
// Alpha interactive background (active)
inline constexpr Color GreenA5{ 43, 255, 162, 60 };
// Alpha border (subtle)
inline constexpr Color GreenA6{ 68, 255, 170, 75 };
// Alpha border
inline constexpr Color GreenA7{ 80, 253, 172, 94 };
// Alpha border (strong)
inline constexpr Color GreenA8{ 84, 255, 173, 115 };
// Alpha solid
inline constexpr Color GreenA9{ 68, 255, 164, 158 };
// Alpha solid (hover)
inline constexpr Color GreenA10{ 67, 254, 164, 171 };
// Alpha text
inline constexpr Color GreenA11{ 70, 254, 165, 212 };
// Alpha text (high contrast)
inline constexpr Color GreenA12{ 187, 255, 215, 240 };

// Background
inline constexpr Color Grass1{ 14, 21, 17, 255 };
// Background (subtle)
inline constexpr Color Grass2{ 20, 26, 21, 255 };
// Interactive background
inline constexpr Color Grass3{ 27, 42, 30, 255 };
// Interactive background (hover)
inline constexpr Color Grass4{ 29, 58, 36, 255 };
// Interactive background (active)
inline constexpr Color Grass5{ 37, 72, 45, 255 };
// Border (subtle)
inline constexpr Color Grass6{ 45, 87, 54, 255 };
// Border
inline constexpr Color Grass7{ 54, 103, 64, 255 };
// Border (strong)
inline constexpr Color Grass8{ 62, 121, 73, 255 };
// Solid
inline constexpr Color Grass9{ 70, 167, 88, 255 };
// Solid (hover)
inline constexpr Color Grass10{ 83, 179, 101, 255 };
// Text
inline constexpr Color Grass11{ 113, 208, 131, 255 };
// Text (high contrast)
inline constexpr Color Grass12{ 194, 240, 194, 255 };

// Alpha background
inline constexpr Color GrassA1{ 0, 222, 18, 5 };
// Alpha background (subtle)
inline constexpr Color GrassA2{ 94, 247, 120, 10 };
// Alpha interactive background
inline constexpr Color GrassA3{ 112, 254, 140, 27 };
// Alpha interactive background (hover)
inline constexpr Color GrassA4{ 87, 255, 128, 44 };
// Alpha interactive background (active)
inline constexpr Color GrassA5{ 104, 255, 139, 59 };
// Alpha border (subtle)
inline constexpr Color GrassA6{ 113, 255, 143, 75 };
// Alpha border
inline constexpr Color GrassA7{ 119, 253, 146, 93 };
// Alpha border (strong)
inline constexpr Color GrassA8{ 119, 253, 144, 112 };
// Alpha solid
inline constexpr Color GrassA9{ 101, 255, 130, 161 };
// Alpha solid (hover)
inline constexpr Color GrassA10{ 114, 255, 141, 174 };
// Alpha text
inline constexpr Color GrassA11{ 137, 255, 159, 205 };
// Alpha text (high contrast)
inline constexpr Color GrassA12{ 206, 255, 206, 239 };

// Background
inline constexpr Color Mint1{ 14, 21, 21, 255 };
// Background (subtle)
inline constexpr Color Mint2{ 15, 27, 27, 255 };
// Interactive background
inline constexpr Color Mint3{ 9, 44, 43, 255 };
// Interactive background (hover)
inline constexpr Color Mint4{ 0, 58, 56, 255 };
// Interactive background (active)
inline constexpr Color Mint5{ 0, 71, 68, 255 };
// Border (subtle)
inline constexpr Color Mint6{ 16, 86, 80, 255 };
// Border
inline constexpr Color Mint7{ 30, 104, 95, 255 };
// Border (strong)
inline constexpr Color Mint8{ 39, 127, 112, 255 };
// Solid
inline constexpr Color Mint9{ 134, 234, 212, 255 };
// Solid (hover)
inline constexpr Color Mint10{ 168, 245, 229, 255 };
// Text
inline constexpr Color Mint11{ 88, 213, 186, 255 };
// Text (high contrast)
inline constexpr Color Mint12{ 196, 245, 225, 255 };

// Alpha background
inline constexpr Color MintA1{ 0, 222, 222, 5 };
// Alpha background (subtle)
inline constexpr Color MintA2{ 0, 249, 249, 11 };
// Alpha interactive background
inline constexpr Color MintA3{ 0, 255, 246, 29 };
// Alpha interactive background (hover)
inline constexpr Color MintA4{ 0, 255, 244, 44 };
// Alpha interactive background (active)
inline constexpr Color MintA5{ 0, 255, 242, 58 };
// Alpha border (subtle)
inline constexpr Color MintA6{ 14, 255, 235, 74 };
// Alpha border
inline constexpr Color MintA7{ 52, 253, 229, 94 };
// Alpha border (strong)
inline constexpr Color MintA8{ 65, 255, 223, 118 };
// Alpha solid
inline constexpr Color MintA9{ 146, 255, 231, 233 };
// Alpha solid (hover)
inline constexpr Color MintA10{ 174, 254, 237, 245 };
// Alpha text
inline constexpr Color MintA11{ 103, 255, 222, 210 };
// Alpha text (high contrast)
inline constexpr Color MintA12{ 203, 254, 233, 245 };

// Background
inline constexpr Color Lime1{ 17, 19, 12, 255 };
// Background (subtle)
inline constexpr Color Lime2{ 21, 26, 16, 255 };
// Interactive background
inline constexpr Color Lime3{ 31, 41, 23, 255 };
// Interactive background (hover)
inline constexpr Color Lime4{ 41, 55, 29, 255 };
// Interactive background (active)
inline constexpr Color Lime5{ 51, 68, 35, 255 };
// Border (subtle)
inline constexpr Color Lime6{ 61, 82, 42, 255 };
// Border
inline constexpr Color Lime7{ 73, 98, 49, 255 };
// Border (strong)
inline constexpr Color Lime8{ 87, 117, 56, 255 };
// Solid
inline constexpr Color Lime9{ 189, 238, 99, 255 };
// Solid (hover)
inline constexpr Color Lime10{ 212, 255, 112, 255 };
// Text
inline constexpr Color Lime11{ 189, 229, 108, 255 };
// Text (high contrast)
inline constexpr Color Lime12{ 227, 247, 186, 255 };

// Alpha background
inline constexpr Color LimeA1{ 17, 187, 0, 3 };
// Alpha background (subtle)
inline constexpr Color LimeA2{ 120, 247, 0, 10 };
// Alpha interactive background
inline constexpr Color LimeA3{ 155, 253, 76, 26 };
// Alpha interactive background (hover)
inline constexpr Color LimeA4{ 167, 254, 92, 41 };
// Alpha interactive background (active)
inline constexpr Color LimeA5{ 175, 254, 101, 55 };
// Alpha border (subtle)
inline constexpr Color LimeA6{ 178, 254, 109, 70 };
// Alpha border
inline constexpr Color LimeA7{ 182, 255, 111, 87 };
// Alpha border (strong)
inline constexpr Color LimeA8{ 182, 253, 109, 108 };
// Alpha solid
inline constexpr Color LimeA9{ 202, 255, 105, 237 };
// Alpha solid (hover)
inline constexpr Color LimeA10{ 212, 255, 112, 255 };
// Alpha text
inline constexpr Color LimeA11{ 209, 254, 119, 228 };
// Alpha text (high contrast)
inline constexpr Color LimeA12{ 233, 254, 191, 247 };

// Background
inline constexpr Color Yellow1{ 20, 18, 11, 255 };
// Background (subtle)
inline constexpr Color Yellow2{ 27, 24, 15, 255 };
// Interactive background
inline constexpr Color Yellow3{ 45, 35, 5, 255 };
// Interactive background (hover)
inline constexpr Color Yellow4{ 54, 43, 0, 255 };
// Interactive background (active)
inline constexpr Color Yellow5{ 67, 53, 0, 255 };
// Border (subtle)
inline constexpr Color Yellow6{ 82, 66, 2, 255 };
// Border
inline constexpr Color Yellow7{ 102, 84, 23, 255 };
// Border (strong)
inline constexpr Color Yellow8{ 131, 106, 33, 255 };
// Solid
inline constexpr Color Yellow9{ 255, 230, 41, 255 };
// Solid (hover)
inline constexpr Color Yellow10{ 255, 255, 87, 255 };
// Text
inline constexpr Color Yellow11{ 245, 225, 71, 255 };
// Text (high contrast)
inline constexpr Color Yellow12{ 246, 238, 180, 255 };

// Alpha background
inline constexpr Color YellowA1{ 209, 81, 0, 4 };
// Alpha background (subtle)
inline constexpr Color YellowA2{ 249, 180, 0, 11 };
// Alpha interactive background
inline constexpr Color YellowA3{ 255, 170, 0, 30 };
// Alpha interactive background (hover)
inline constexpr Color YellowA4{ 253, 183, 0, 40 };
// Alpha interactive background (active)
inline constexpr Color YellowA5{ 254, 187, 0, 54 };
// Alpha border (subtle)
inline constexpr Color YellowA6{ 254, 196, 0, 70 };
// Alpha border
inline constexpr Color YellowA7{ 253, 203, 34, 92 };
// Alpha border (strong)
inline constexpr Color YellowA8{ 253, 202, 50, 123 };
// Alpha solid
inline constexpr Color YellowA9{ 255, 230, 41, 255 };
// Alpha solid (hover)
inline constexpr Color YellowA10{ 255, 255, 87, 255 };
// Alpha text
inline constexpr Color YellowA11{ 254, 233, 73, 245 };
// Alpha text (high contrast)
inline constexpr Color YellowA12{ 254, 246, 186, 246 };

// Background
inline constexpr Color Amber1{ 22, 18, 12, 255 };
// Background (subtle)
inline constexpr Color Amber2{ 29, 24, 15, 255 };
// Interactive background
inline constexpr Color Amber3{ 48, 32, 8, 255 };
// Interactive background (hover)
inline constexpr Color Amber4{ 63, 39, 0, 255 };
// Interactive background (active)
inline constexpr Color Amber5{ 77, 48, 0, 255 };
// Border (subtle)
inline constexpr Color Amber6{ 92, 61, 5, 255 };
// Border
inline constexpr Color Amber7{ 113, 79, 25, 255 };
// Border (strong)
inline constexpr Color Amber8{ 143, 100, 36, 255 };
// Solid
inline constexpr Color Amber9{ 255, 197, 61, 255 };
// Solid (hover)
inline constexpr Color Amber10{ 255, 214, 10, 255 };
// Text
inline constexpr Color Amber11{ 255, 202, 22, 255 };
// Text (high contrast)
inline constexpr Color Amber12{ 255, 231, 179, 255 };

// Alpha background
inline constexpr Color AmberA1{ 230, 60, 0, 6 };
// Alpha background (subtle)
inline constexpr Color AmberA2{ 253, 155, 0, 13 };
// Alpha interactive background
inline constexpr Color AmberA3{ 250, 130, 0, 34 };
// Alpha interactive background (hover)
inline constexpr Color AmberA4{ 252, 130, 0, 50 };
// Alpha interactive background (active)
inline constexpr Color AmberA5{ 253, 139, 0, 65 };
// Alpha border (subtle)
inline constexpr Color AmberA6{ 253, 155, 0, 81 };
// Alpha border
inline constexpr Color AmberA7{ 255, 171, 37, 103 };
// Alpha border (strong)
inline constexpr Color AmberA8{ 255, 174, 53, 135 };
// Alpha solid
inline constexpr Color AmberA9{ 255, 197, 61, 255 };
// Alpha solid (hover)
inline constexpr Color AmberA10{ 255, 214, 10, 255 };
// Alpha text
inline constexpr Color AmberA11{ 255, 202, 22, 255 };
// Alpha text (high contrast)
inline constexpr Color AmberA12{ 255, 231, 179, 255 };

// Background
inline constexpr Color Orange1{ 23, 18, 14, 255 };
// Background (subtle)
inline constexpr Color Orange2{ 30, 22, 15, 255 };
// Interactive background
inline constexpr Color Orange3{ 51, 30, 11, 255 };
// Interactive background (hover)
inline constexpr Color Orange4{ 70, 33, 0, 255 };
// Interactive background (active)
inline constexpr Color Orange5{ 86, 40, 0, 255 };
// Border (subtle)
inline constexpr Color Orange6{ 102, 53, 12, 255 };
// Border
inline constexpr Color Orange7{ 126, 69, 29, 255 };
// Border (strong)
inline constexpr Color Orange8{ 163, 88, 41, 255 };
// Solid
inline constexpr Color Orange9{ 247, 107, 21, 255 };
// Solid (hover)
inline constexpr Color Orange10{ 255, 128, 31, 255 };
// Text
inline constexpr Color Orange11{ 255, 160, 87, 255 };
// Text (high contrast)
inline constexpr Color Orange12{ 255, 224, 194, 255 };

// Alpha background
inline constexpr Color OrangeA1{ 236, 54, 0, 7 };
// Alpha background (subtle)
inline constexpr Color OrangeA2{ 254, 109, 0, 14 };
// Alpha interactive background
inline constexpr Color OrangeA3{ 251, 106, 0, 37 };
// Alpha interactive background (hover)
inline constexpr Color OrangeA4{ 255, 89, 0, 57 };
// Alpha interactive background (active)
inline constexpr Color OrangeA5{ 255, 97, 0, 74 };
// Alpha border (subtle)
inline constexpr Color OrangeA6{ 253, 117, 4, 92 };
// Alpha border
inline constexpr Color OrangeA7{ 255, 131, 44, 117 };
// Alpha border (strong)
inline constexpr Color OrangeA8{ 254, 132, 56, 157 };
// Alpha solid
inline constexpr Color OrangeA9{ 254, 109, 21, 247 };
// Alpha solid (hover)
inline constexpr Color OrangeA10{ 255, 128, 31, 255 };
// Alpha text
inline constexpr Color OrangeA11{ 255, 160, 87, 255 };
// Alpha text (high contrast)
inline constexpr Color OrangeA12{ 255, 224, 194, 255 };

// Background
inline constexpr Color Brown1{ 18, 17, 15, 255 };
// Background (subtle)
inline constexpr Color Brown2{ 28, 24, 22, 255 };
// Interactive background
inline constexpr Color Brown3{ 40, 33, 29, 255 };
// Interactive background (hover)
inline constexpr Color Brown4{ 50, 41, 34, 255 };
// Interactive background (active)
inline constexpr Color Brown5{ 62, 49, 40, 255 };
// Border (subtle)
inline constexpr Color Brown6{ 77, 60, 47, 255 };
// Border
inline constexpr Color Brown7{ 97, 74, 57, 255 };
// Border (strong)
inline constexpr Color Brown8{ 124, 95, 70, 255 };
// Solid
inline constexpr Color Brown9{ 173, 127, 88, 255 };
// Solid (hover)
inline constexpr Color Brown10{ 184, 140, 103, 255 };
// Text
inline constexpr Color Brown11{ 219, 181, 148, 255 };
// Text (high contrast)
inline constexpr Color Brown12{ 242, 225, 202, 255 };

// Alpha background
inline constexpr Color BrownA1{ 145, 17, 0, 2 };
// Alpha background (subtle)
inline constexpr Color BrownA2{ 251, 166, 124, 12 };
// Alpha interactive background
inline constexpr Color BrownA3{ 252, 181, 140, 25 };
// Alpha interactive background (hover)
inline constexpr Color BrownA4{ 251, 187, 138, 36 };
// Alpha interactive background (active)
inline constexpr Color BrownA5{ 252, 184, 137, 49 };
// Alpha border (subtle)
inline constexpr Color BrownA6{ 253, 186, 135, 65 };
// Alpha border
inline constexpr Color BrownA7{ 255, 187, 136, 86 };
// Alpha border (strong)
inline constexpr Color BrownA8{ 255, 190, 135, 115 };
// Alpha solid
inline constexpr Color BrownA9{ 254, 184, 125, 168 };
// Alpha solid (hover)
inline constexpr Color BrownA10{ 255, 193, 140, 179 };
// Alpha text
inline constexpr Color BrownA11{ 254, 209, 170, 217 };
// Alpha text (high contrast)
inline constexpr Color BrownA12{ 254, 236, 212, 242 };

// Background
inline constexpr Color Bronze1{ 20, 17, 16, 255 };
// Background (subtle)
inline constexpr Color Bronze2{ 28, 25, 23, 255 };
// Interactive background
inline constexpr Color Bronze3{ 38, 34, 32, 255 };
// Interactive background (hover)
inline constexpr Color Bronze4{ 48, 42, 39, 255 };
// Interactive background (active)
inline constexpr Color Bronze5{ 59, 51, 48, 255 };
// Border (subtle)
inline constexpr Color Bronze6{ 73, 62, 58, 255 };
// Border
inline constexpr Color Bronze7{ 90, 76, 71, 255 };
// Border (strong)
inline constexpr Color Bronze8{ 111, 95, 88, 255 };
// Solid
inline constexpr Color Bronze9{ 161, 128, 114, 255 };
// Solid (hover)
inline constexpr Color Bronze10{ 174, 140, 126, 255 };
// Text
inline constexpr Color Bronze11{ 212, 179, 165, 255 };
// Text (high contrast)
inline constexpr Color Bronze12{ 237, 224, 217, 255 };

// Alpha background
inline constexpr Color BronzeA1{ 209, 17, 0, 4 };
// Alpha background (subtle)
inline constexpr Color BronzeA2{ 251, 188, 145, 12 };
// Alpha interactive background
inline constexpr Color BronzeA3{ 250, 206, 184, 23 };
// Alpha interactive background (hover)
inline constexpr Color BronzeA4{ 250, 205, 182, 34 };
// Alpha interactive background (active)
inline constexpr Color BronzeA5{ 255, 210, 193, 45 };
// Alpha border (subtle)
inline constexpr Color BronzeA6{ 255, 209, 192, 60 };
// Alpha border
inline constexpr Color BronzeA7{ 253, 208, 192, 79 };
// Alpha border (strong)
inline constexpr Color BronzeA8{ 255, 214, 197, 101 };
// Alpha solid
inline constexpr Color BronzeA9{ 254, 199, 176, 155 };
// Alpha solid (hover)
inline constexpr Color BronzeA10{ 254, 202, 181, 169 };
// Alpha text
inline constexpr Color BronzeA11{ 255, 215, 198, 209 };
// Alpha text (high contrast)
inline constexpr Color BronzeA12{ 255, 241, 233, 236 };

// Background
inline constexpr Color Gold1{ 18, 18, 17, 255 };
// Background (subtle)
inline constexpr Color Gold2{ 27, 26, 23, 255 };
// Interactive background
inline constexpr Color Gold3{ 36, 35, 31, 255 };
// Interactive background (hover)
inline constexpr Color Gold4{ 45, 43, 38, 255 };
// Interactive background (active)
inline constexpr Color Gold5{ 56, 53, 46, 255 };
// Border (subtle)
inline constexpr Color Gold6{ 68, 64, 57, 255 };
// Border
inline constexpr Color Gold7{ 84, 79, 70, 255 };
// Border (strong)
inline constexpr Color Gold8{ 105, 98, 86, 255 };
// Solid
inline constexpr Color Gold9{ 151, 131, 101, 255 };
// Solid (hover)
inline constexpr Color Gold10{ 163, 144, 115, 255 };
// Text
inline constexpr Color Gold11{ 203, 185, 159, 255 };
// Text (high contrast)
inline constexpr Color Gold12{ 232, 226, 217, 255 };

// Alpha background
inline constexpr Color GoldA1{ 145, 145, 17, 2 };
// Alpha background (subtle)
inline constexpr Color GoldA2{ 249, 226, 157, 11 };
// Alpha interactive background
inline constexpr Color GoldA3{ 248, 236, 187, 21 };
// Alpha interactive background (hover)
inline constexpr Color GoldA4{ 255, 238, 196, 30 };
// Alpha interactive background (active)
inline constexpr Color GoldA5{ 254, 236, 194, 42 };
// Alpha border (subtle)
inline constexpr Color GoldA6{ 254, 235, 203, 55 };
// Alpha border
inline constexpr Color GoldA7{ 255, 237, 205, 72 };
// Alpha border (strong)
inline constexpr Color GoldA8{ 253, 234, 202, 95 };
// Alpha solid
inline constexpr Color GoldA9{ 255, 219, 166, 144 };
// Alpha solid (hover)
inline constexpr Color GoldA10{ 254, 223, 176, 157 };
// Alpha text
inline constexpr Color GoldA11{ 254, 231, 198, 200 };
// Alpha text (high contrast)
inline constexpr Color GoldA12{ 254, 247, 237, 231 };
} // namespace onyx::colors::dark

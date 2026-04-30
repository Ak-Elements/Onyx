#pragma once

// Radix UI Color System - Light mode (solid + alpha scales)
// Full reference: https://www.radix-ui.com/colors
// Each scale has 12 steps:
//   1-2:  Backgrounds
//   3-5:  Interactive component backgrounds
//   6-8:  Borders and separators
//   9-10: Solid colors
//   11:   Accessible text
//   12:   High-contrast text
namespace onyx::colors::light {

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
inline constexpr Color Gray1{ 252, 252, 252, 255 };
// Background (subtle)
inline constexpr Color Gray2{ 249, 249, 249, 255 };
// Interactive background
inline constexpr Color Gray3{ 240, 240, 240, 255 };
// Interactive background (hover)
inline constexpr Color Gray4{ 232, 232, 232, 255 };
// Interactive background (active)
inline constexpr Color Gray5{ 224, 224, 224, 255 };
// Border (subtle)
inline constexpr Color Gray6{ 217, 217, 217, 255 };
// Border
inline constexpr Color Gray7{ 206, 206, 206, 255 };
// Border (strong)
inline constexpr Color Gray8{ 187, 187, 187, 255 };
// Solid
inline constexpr Color Gray9{ 141, 141, 141, 255 };
// Solid (hover)
inline constexpr Color Gray10{ 131, 131, 131, 255 };
// Text
inline constexpr Color Gray11{ 100, 100, 100, 255 };
// Text (high contrast)
inline constexpr Color Gray12{ 32, 32, 32, 255 };

// Alpha background
inline constexpr Color GrayA1{ 0, 0, 0, 3 };
// Alpha background (subtle)
inline constexpr Color GrayA2{ 0, 0, 0, 6 };
// Alpha interactive background
inline constexpr Color GrayA3{ 0, 0, 0, 15 };
// Alpha interactive background (hover)
inline constexpr Color GrayA4{ 0, 0, 0, 23 };
// Alpha interactive background (active)
inline constexpr Color GrayA5{ 0, 0, 0, 31 };
// Alpha border (subtle)
inline constexpr Color GrayA6{ 0, 0, 0, 38 };
// Alpha border
inline constexpr Color GrayA7{ 0, 0, 0, 49 };
// Alpha border (strong)
inline constexpr Color GrayA8{ 0, 0, 0, 68 };
// Alpha solid
inline constexpr Color GrayA9{ 0, 0, 0, 114 };
// Alpha solid (hover)
inline constexpr Color GrayA10{ 0, 0, 0, 124 };
// Alpha text
inline constexpr Color GrayA11{ 0, 0, 0, 155 };
// Alpha text (high contrast)
inline constexpr Color GrayA12{ 0, 0, 0, 223 };

// Background
inline constexpr Color Mauve1{ 253, 252, 253, 255 };
// Background (subtle)
inline constexpr Color Mauve2{ 250, 249, 251, 255 };
// Interactive background
inline constexpr Color Mauve3{ 242, 239, 243, 255 };
// Interactive background (hover)
inline constexpr Color Mauve4{ 234, 231, 236, 255 };
// Interactive background (active)
inline constexpr Color Mauve5{ 227, 223, 230, 255 };
// Border (subtle)
inline constexpr Color Mauve6{ 219, 216, 224, 255 };
// Border
inline constexpr Color Mauve7{ 208, 205, 215, 255 };
// Border (strong)
inline constexpr Color Mauve8{ 188, 186, 199, 255 };
// Solid
inline constexpr Color Mauve9{ 142, 140, 153, 255 };
// Solid (hover)
inline constexpr Color Mauve10{ 132, 130, 142, 255 };
// Text
inline constexpr Color Mauve11{ 101, 99, 109, 255 };
// Text (high contrast)
inline constexpr Color Mauve12{ 33, 31, 38, 255 };

// Alpha background
inline constexpr Color MauveA1{ 85, 0, 85, 3 };
// Alpha background (subtle)
inline constexpr Color MauveA2{ 43, 0, 85, 6 };
// Alpha interactive background
inline constexpr Color MauveA3{ 48, 0, 64, 16 };
// Alpha interactive background (hover)
inline constexpr Color MauveA4{ 32, 0, 54, 24 };
// Alpha interactive background (active)
inline constexpr Color MauveA5{ 32, 0, 56, 32 };
// Alpha border (subtle)
inline constexpr Color MauveA6{ 20, 0, 53, 39 };
// Alpha border
inline constexpr Color MauveA7{ 16, 0, 51, 50 };
// Alpha border (strong)
inline constexpr Color MauveA8{ 8, 0, 49, 69 };
// Alpha solid
inline constexpr Color MauveA9{ 5, 0, 29, 115 };
// Alpha solid (hover)
inline constexpr Color MauveA10{ 5, 0, 25, 125 };
// Alpha text
inline constexpr Color MauveA11{ 4, 0, 17, 156 };
// Alpha text (high contrast)
inline constexpr Color MauveA12{ 2, 0, 8, 224 };

// Background
inline constexpr Color Slate1{ 252, 252, 253, 255 };
// Background (subtle)
inline constexpr Color Slate2{ 249, 249, 251, 255 };
// Interactive background
inline constexpr Color Slate3{ 240, 240, 243, 255 };
// Interactive background (hover)
inline constexpr Color Slate4{ 232, 232, 236, 255 };
// Interactive background (active)
inline constexpr Color Slate5{ 224, 225, 230, 255 };
// Border (subtle)
inline constexpr Color Slate6{ 217, 217, 224, 255 };
// Border
inline constexpr Color Slate7{ 205, 206, 214, 255 };
// Border (strong)
inline constexpr Color Slate8{ 185, 187, 198, 255 };
// Solid
inline constexpr Color Slate9{ 139, 141, 152, 255 };
// Solid (hover)
inline constexpr Color Slate10{ 128, 131, 141, 255 };
// Text
inline constexpr Color Slate11{ 96, 100, 108, 255 };
// Text (high contrast)
inline constexpr Color Slate12{ 28, 32, 36, 255 };

// Alpha background
inline constexpr Color SlateA1{ 0, 0, 85, 3 };
// Alpha background (subtle)
inline constexpr Color SlateA2{ 0, 0, 85, 6 };
// Alpha interactive background
inline constexpr Color SlateA3{ 0, 0, 51, 15 };
// Alpha interactive background (hover)
inline constexpr Color SlateA4{ 0, 0, 45, 23 };
// Alpha interactive background (active)
inline constexpr Color SlateA5{ 0, 9, 50, 31 };
// Alpha border (subtle)
inline constexpr Color SlateA6{ 0, 0, 47, 38 };
// Alpha border
inline constexpr Color SlateA7{ 0, 6, 46, 50 };
// Alpha border (strong)
inline constexpr Color SlateA8{ 0, 8, 48, 70 };
// Alpha solid
inline constexpr Color SlateA9{ 0, 5, 29, 116 };
// Alpha solid (hover)
inline constexpr Color SlateA10{ 0, 7, 27, 127 };
// Alpha text
inline constexpr Color SlateA11{ 0, 7, 20, 159 };
// Alpha text (high contrast)
inline constexpr Color SlateA12{ 0, 5, 9, 227 };

// Background
inline constexpr Color Sage1{ 251, 253, 252, 255 };
// Background (subtle)
inline constexpr Color Sage2{ 247, 249, 248, 255 };
// Interactive background
inline constexpr Color Sage3{ 238, 241, 240, 255 };
// Interactive background (hover)
inline constexpr Color Sage4{ 230, 233, 232, 255 };
// Interactive background (active)
inline constexpr Color Sage5{ 223, 226, 224, 255 };
// Border (subtle)
inline constexpr Color Sage6{ 215, 218, 217, 255 };
// Border
inline constexpr Color Sage7{ 203, 207, 205, 255 };
// Border (strong)
inline constexpr Color Sage8{ 184, 188, 186, 255 };
// Solid
inline constexpr Color Sage9{ 134, 142, 139, 255 };
// Solid (hover)
inline constexpr Color Sage10{ 124, 132, 129, 255 };
// Text
inline constexpr Color Sage11{ 95, 101, 99, 255 };
// Text (high contrast)
inline constexpr Color Sage12{ 26, 33, 30, 255 };

// Alpha background
inline constexpr Color SageA1{ 0, 128, 64, 4 };
// Alpha background (subtle)
inline constexpr Color SageA2{ 0, 64, 32, 8 };
// Alpha interactive background
inline constexpr Color SageA3{ 0, 45, 30, 17 };
// Alpha interactive background (hover)
inline constexpr Color SageA4{ 0, 31, 21, 25 };
// Alpha interactive background (active)
inline constexpr Color SageA5{ 0, 24, 8, 32 };
// Alpha border (subtle)
inline constexpr Color SageA6{ 0, 20, 13, 40 };
// Alpha border
inline constexpr Color SageA7{ 0, 20, 10, 52 };
// Alpha border (strong)
inline constexpr Color SageA8{ 0, 15, 8, 71 };
// Alpha solid
inline constexpr Color SageA9{ 0, 17, 11, 121 };
// Alpha solid (hover)
inline constexpr Color SageA10{ 0, 16, 10, 131 };
// Alpha text
inline constexpr Color SageA11{ 0, 10, 7, 160 };
// Alpha text (high contrast)
inline constexpr Color SageA12{ 0, 8, 5, 229 };

// Background
inline constexpr Color Olive1{ 252, 253, 252, 255 };
// Background (subtle)
inline constexpr Color Olive2{ 248, 250, 248, 255 };
// Interactive background
inline constexpr Color Olive3{ 239, 241, 239, 255 };
// Interactive background (hover)
inline constexpr Color Olive4{ 231, 233, 231, 255 };
// Interactive background (active)
inline constexpr Color Olive5{ 223, 226, 223, 255 };
// Border (subtle)
inline constexpr Color Olive6{ 215, 218, 215, 255 };
// Border
inline constexpr Color Olive7{ 204, 207, 204, 255 };
// Border (strong)
inline constexpr Color Olive8{ 185, 188, 184, 255 };
// Solid
inline constexpr Color Olive9{ 137, 142, 135, 255 };
// Solid (hover)
inline constexpr Color Olive10{ 127, 132, 125, 255 };
// Text
inline constexpr Color Olive11{ 96, 101, 95, 255 };
// Text (high contrast)
inline constexpr Color Olive12{ 29, 33, 28, 255 };

// Alpha background
inline constexpr Color OliveA1{ 0, 85, 0, 3 };
// Alpha background (subtle)
inline constexpr Color OliveA2{ 0, 73, 0, 7 };
// Alpha interactive background
inline constexpr Color OliveA3{ 0, 32, 0, 16 };
// Alpha interactive background (hover)
inline constexpr Color OliveA4{ 0, 22, 0, 24 };
// Alpha interactive background (active)
inline constexpr Color OliveA5{ 0, 24, 0, 32 };
// Alpha border (subtle)
inline constexpr Color OliveA6{ 0, 20, 0, 40 };
// Alpha border
inline constexpr Color OliveA7{ 0, 15, 0, 51 };
// Alpha border (strong)
inline constexpr Color OliveA8{ 4, 15, 0, 71 };
// Alpha solid
inline constexpr Color OliveA9{ 5, 15, 0, 120 };
// Alpha solid (hover)
inline constexpr Color OliveA10{ 4, 14, 0, 130 };
// Alpha text
inline constexpr Color OliveA11{ 2, 10, 0, 160 };
// Alpha text (high contrast)
inline constexpr Color OliveA12{ 1, 6, 0, 227 };

// Background
inline constexpr Color Sand1{ 253, 253, 252, 255 };
// Background (subtle)
inline constexpr Color Sand2{ 249, 249, 248, 255 };
// Interactive background
inline constexpr Color Sand3{ 241, 240, 239, 255 };
// Interactive background (hover)
inline constexpr Color Sand4{ 233, 232, 230, 255 };
// Interactive background (active)
inline constexpr Color Sand5{ 226, 225, 222, 255 };
// Border (subtle)
inline constexpr Color Sand6{ 218, 217, 214, 255 };
// Border
inline constexpr Color Sand7{ 207, 206, 202, 255 };
// Border (strong)
inline constexpr Color Sand8{ 188, 187, 181, 255 };
// Solid
inline constexpr Color Sand9{ 141, 141, 134, 255 };
// Solid (hover)
inline constexpr Color Sand10{ 130, 130, 124, 255 };
// Text
inline constexpr Color Sand11{ 99, 99, 94, 255 };
// Text (high contrast)
inline constexpr Color Sand12{ 33, 32, 28, 255 };

// Alpha background
inline constexpr Color SandA1{ 85, 85, 0, 3 };
// Alpha background (subtle)
inline constexpr Color SandA2{ 37, 37, 0, 7 };
// Alpha interactive background
inline constexpr Color SandA3{ 32, 16, 0, 16 };
// Alpha interactive background (hover)
inline constexpr Color SandA4{ 31, 21, 0, 25 };
// Alpha interactive background (active)
inline constexpr Color SandA5{ 31, 24, 0, 33 };
// Alpha border (subtle)
inline constexpr Color SandA6{ 25, 19, 0, 41 };
// Alpha border
inline constexpr Color SandA7{ 25, 20, 0, 53 };
// Alpha border (strong)
inline constexpr Color SandA8{ 25, 21, 1, 74 };
// Alpha solid
inline constexpr Color SandA9{ 15, 15, 0, 121 };
// Alpha solid (hover)
inline constexpr Color SandA10{ 12, 12, 0, 131 };
// Alpha text
inline constexpr Color SandA11{ 8, 8, 0, 161 };
// Alpha text (high contrast)
inline constexpr Color SandA12{ 6, 5, 0, 227 };

// Background
inline constexpr Color Tomato1{ 255, 252, 252, 255 };
// Background (subtle)
inline constexpr Color Tomato2{ 255, 248, 247, 255 };
// Interactive background
inline constexpr Color Tomato3{ 254, 235, 231, 255 };
// Interactive background (hover)
inline constexpr Color Tomato4{ 255, 220, 211, 255 };
// Interactive background (active)
inline constexpr Color Tomato5{ 255, 205, 194, 255 };
// Border (subtle)
inline constexpr Color Tomato6{ 253, 189, 175, 255 };
// Border
inline constexpr Color Tomato7{ 245, 168, 152, 255 };
// Border (strong)
inline constexpr Color Tomato8{ 236, 142, 123, 255 };
// Solid
inline constexpr Color Tomato9{ 229, 77, 46, 255 };
// Solid (hover)
inline constexpr Color Tomato10{ 221, 68, 37, 255 };
// Text
inline constexpr Color Tomato11{ 209, 52, 21, 255 };
// Text (high contrast)
inline constexpr Color Tomato12{ 92, 39, 31, 255 };

// Alpha background
inline constexpr Color TomatoA1{ 255, 0, 0, 3 };
// Alpha background (subtle)
inline constexpr Color TomatoA2{ 255, 32, 0, 8 };
// Alpha interactive background
inline constexpr Color TomatoA3{ 245, 43, 0, 24 };
// Alpha interactive background (hover)
inline constexpr Color TomatoA4{ 255, 53, 0, 44 };
// Alpha interactive background (active)
inline constexpr Color TomatoA5{ 255, 46, 0, 61 };
// Alpha border (subtle)
inline constexpr Color TomatoA6{ 249, 45, 0, 80 };
// Alpha border
inline constexpr Color TomatoA7{ 231, 40, 0, 103 };
// Alpha border (strong)
inline constexpr Color TomatoA8{ 219, 37, 0, 132 };
// Alpha solid
inline constexpr Color TomatoA9{ 223, 38, 0, 209 };
// Alpha solid (hover)
inline constexpr Color TomatoA10{ 215, 36, 0, 218 };
// Alpha text
inline constexpr Color TomatoA11{ 205, 34, 0, 234 };
// Alpha text (high contrast)
inline constexpr Color TomatoA12{ 70, 9, 0, 224 };

// Background
inline constexpr Color Red1{ 255, 252, 252, 255 };
// Background (subtle)
inline constexpr Color Red2{ 255, 247, 247, 255 };
// Interactive background
inline constexpr Color Red3{ 254, 235, 236, 255 };
// Interactive background (hover)
inline constexpr Color Red4{ 255, 219, 220, 255 };
// Interactive background (active)
inline constexpr Color Red5{ 255, 205, 206, 255 };
// Border (subtle)
inline constexpr Color Red6{ 253, 189, 190, 255 };
// Border
inline constexpr Color Red7{ 244, 169, 170, 255 };
// Border (strong)
inline constexpr Color Red8{ 235, 142, 144, 255 };
// Solid
inline constexpr Color Red9{ 229, 72, 77, 255 };
// Solid (hover)
inline constexpr Color Red10{ 220, 62, 66, 255 };
// Text
inline constexpr Color Red11{ 206, 44, 49, 255 };
// Text (high contrast)
inline constexpr Color Red12{ 100, 23, 35, 255 };

// Alpha background
inline constexpr Color RedA1{ 255, 0, 0, 3 };
// Alpha background (subtle)
inline constexpr Color RedA2{ 255, 0, 0, 8 };
// Alpha interactive background
inline constexpr Color RedA3{ 243, 0, 13, 20 };
// Alpha interactive background (hover)
inline constexpr Color RedA4{ 255, 0, 8, 36 };
// Alpha interactive background (active)
inline constexpr Color RedA5{ 255, 0, 6, 50 };
// Alpha border (subtle)
inline constexpr Color RedA6{ 248, 0, 4, 66 };
// Alpha border
inline constexpr Color RedA7{ 223, 0, 3, 86 };
// Alpha border (strong)
inline constexpr Color RedA8{ 210, 0, 5, 113 };
// Alpha solid
inline constexpr Color RedA9{ 219, 0, 7, 183 };
// Alpha solid (hover)
inline constexpr Color RedA10{ 209, 0, 5, 193 };
// Alpha text
inline constexpr Color RedA11{ 196, 0, 6, 211 };
// Alpha text (high contrast)
inline constexpr Color RedA12{ 85, 0, 13, 232 };

// Background
inline constexpr Color Ruby1{ 255, 252, 253, 255 };
// Background (subtle)
inline constexpr Color Ruby2{ 255, 247, 248, 255 };
// Interactive background
inline constexpr Color Ruby3{ 254, 234, 237, 255 };
// Interactive background (hover)
inline constexpr Color Ruby4{ 255, 220, 225, 255 };
// Interactive background (active)
inline constexpr Color Ruby5{ 255, 206, 214, 255 };
// Border (subtle)
inline constexpr Color Ruby6{ 248, 191, 200, 255 };
// Border
inline constexpr Color Ruby7{ 239, 172, 184, 255 };
// Border (strong)
inline constexpr Color Ruby8{ 229, 146, 163, 255 };
// Solid
inline constexpr Color Ruby9{ 229, 70, 102, 255 };
// Solid (hover)
inline constexpr Color Ruby10{ 220, 59, 93, 255 };
// Text
inline constexpr Color Ruby11{ 202, 36, 77, 255 };
// Text (high contrast)
inline constexpr Color Ruby12{ 100, 23, 43, 255 };

// Alpha background
inline constexpr Color RubyA1{ 255, 0, 85, 3 };
// Alpha background (subtle)
inline constexpr Color RubyA2{ 255, 0, 32, 8 };
// Alpha interactive background
inline constexpr Color RubyA3{ 243, 0, 37, 21 };
// Alpha interactive background (hover)
inline constexpr Color RubyA4{ 255, 0, 37, 35 };
// Alpha interactive background (active)
inline constexpr Color RubyA5{ 255, 0, 42, 49 };
// Alpha border (subtle)
inline constexpr Color RubyA6{ 228, 0, 36, 64 };
// Alpha border
inline constexpr Color RubyA7{ 206, 0, 37, 83 };
// Alpha border (strong)
inline constexpr Color RubyA8{ 195, 0, 40, 109 };
// Alpha solid
inline constexpr Color RubyA9{ 219, 0, 44, 185 };
// Alpha solid (hover)
inline constexpr Color RubyA10{ 210, 0, 44, 196 };
// Alpha text
inline constexpr Color RubyA11{ 193, 0, 48, 219 };
// Alpha text (high contrast)
inline constexpr Color RubyA12{ 85, 0, 22, 232 };

// Background
inline constexpr Color Crimson1{ 255, 252, 253, 255 };
// Background (subtle)
inline constexpr Color Crimson2{ 254, 247, 249, 255 };
// Interactive background
inline constexpr Color Crimson3{ 255, 233, 240, 255 };
// Interactive background (hover)
inline constexpr Color Crimson4{ 254, 220, 231, 255 };
// Interactive background (active)
inline constexpr Color Crimson5{ 250, 206, 221, 255 };
// Border (subtle)
inline constexpr Color Crimson6{ 243, 190, 209, 255 };
// Border
inline constexpr Color Crimson7{ 234, 172, 195, 255 };
// Border (strong)
inline constexpr Color Crimson8{ 224, 147, 178, 255 };
// Solid
inline constexpr Color Crimson9{ 233, 61, 130, 255 };
// Solid (hover)
inline constexpr Color Crimson10{ 223, 52, 120, 255 };
// Text
inline constexpr Color Crimson11{ 203, 29, 99, 255 };
// Text (high contrast)
inline constexpr Color Crimson12{ 98, 22, 57, 255 };

// Alpha background
inline constexpr Color CrimsonA1{ 255, 0, 85, 3 };
// Alpha background (subtle)
inline constexpr Color CrimsonA2{ 224, 0, 64, 8 };
// Alpha interactive background
inline constexpr Color CrimsonA3{ 255, 0, 82, 22 };
// Alpha interactive background (hover)
inline constexpr Color CrimsonA4{ 248, 0, 81, 35 };
// Alpha interactive background (active)
inline constexpr Color CrimsonA5{ 229, 0, 79, 49 };
// Alpha border (subtle)
inline constexpr Color CrimsonA6{ 208, 0, 75, 65 };
// Alpha border
inline constexpr Color CrimsonA7{ 191, 0, 71, 83 };
// Alpha border (strong)
inline constexpr Color CrimsonA8{ 182, 0, 74, 108 };
// Alpha solid
inline constexpr Color CrimsonA9{ 226, 0, 91, 194 };
// Alpha solid (hover)
inline constexpr Color CrimsonA10{ 215, 0, 86, 203 };
// Alpha text
inline constexpr Color CrimsonA11{ 196, 0, 79, 226 };
// Alpha text (high contrast)
inline constexpr Color CrimsonA12{ 83, 0, 38, 233 };

// Background
inline constexpr Color Pink1{ 255, 252, 254, 255 };
// Background (subtle)
inline constexpr Color Pink2{ 254, 247, 251, 255 };
// Interactive background
inline constexpr Color Pink3{ 254, 233, 245, 255 };
// Interactive background (hover)
inline constexpr Color Pink4{ 251, 220, 239, 255 };
// Interactive background (active)
inline constexpr Color Pink5{ 246, 206, 231, 255 };
// Border (subtle)
inline constexpr Color Pink6{ 239, 191, 221, 255 };
// Border
inline constexpr Color Pink7{ 231, 172, 208, 255 };
// Border (strong)
inline constexpr Color Pink8{ 221, 147, 194, 255 };
// Solid
inline constexpr Color Pink9{ 214, 64, 159, 255 };
// Solid (hover)
inline constexpr Color Pink10{ 207, 56, 151, 255 };
// Text
inline constexpr Color Pink11{ 194, 41, 138, 255 };
// Text (high contrast)
inline constexpr Color Pink12{ 101, 18, 73, 255 };

// Alpha background
inline constexpr Color PinkA1{ 255, 0, 170, 3 };
// Alpha background (subtle)
inline constexpr Color PinkA2{ 224, 0, 128, 8 };
// Alpha interactive background
inline constexpr Color PinkA3{ 244, 0, 140, 22 };
// Alpha interactive background (hover)
inline constexpr Color PinkA4{ 226, 0, 139, 35 };
// Alpha interactive background (active)
inline constexpr Color PinkA5{ 209, 0, 131, 49 };
// Alpha border (subtle)
inline constexpr Color PinkA6{ 192, 0, 120, 64 };
// Alpha border
inline constexpr Color PinkA7{ 182, 0, 111, 83 };
// Alpha border (strong)
inline constexpr Color PinkA8{ 175, 0, 111, 108 };
// Alpha solid
inline constexpr Color PinkA9{ 200, 0, 127, 191 };
// Alpha solid (hover)
inline constexpr Color PinkA10{ 194, 0, 122, 199 };
// Alpha text
inline constexpr Color PinkA11{ 182, 0, 116, 214 };
// Alpha text (high contrast)
inline constexpr Color PinkA12{ 89, 0, 59, 237 };

// Background
inline constexpr Color Plum1{ 254, 252, 255, 255 };
// Background (subtle)
inline constexpr Color Plum2{ 253, 247, 253, 255 };
// Interactive background
inline constexpr Color Plum3{ 251, 235, 251, 255 };
// Interactive background (hover)
inline constexpr Color Plum4{ 247, 222, 248, 255 };
// Interactive background (active)
inline constexpr Color Plum5{ 242, 209, 243, 255 };
// Border (subtle)
inline constexpr Color Plum6{ 233, 194, 236, 255 };
// Border
inline constexpr Color Plum7{ 222, 173, 227, 255 };
// Border (strong)
inline constexpr Color Plum8{ 207, 145, 216, 255 };
// Solid
inline constexpr Color Plum9{ 171, 74, 186, 255 };
// Solid (hover)
inline constexpr Color Plum10{ 161, 68, 175, 255 };
// Text
inline constexpr Color Plum11{ 149, 62, 163, 255 };
// Text (high contrast)
inline constexpr Color Plum12{ 83, 25, 93, 255 };

// Alpha background
inline constexpr Color PlumA1{ 170, 0, 255, 3 };
// Alpha background (subtle)
inline constexpr Color PlumA2{ 192, 0, 192, 8 };
// Alpha interactive background
inline constexpr Color PlumA3{ 204, 0, 204, 20 };
// Alpha interactive background (hover)
inline constexpr Color PlumA4{ 194, 0, 201, 33 };
// Alpha interactive background (active)
inline constexpr Color PlumA5{ 183, 0, 189, 46 };
// Alpha border (subtle)
inline constexpr Color PlumA6{ 164, 0, 176, 61 };
// Alpha border
inline constexpr Color PlumA7{ 153, 0, 168, 82 };
// Alpha border (strong)
inline constexpr Color PlumA8{ 144, 0, 165, 110 };
// Alpha solid
inline constexpr Color PlumA9{ 137, 0, 158, 181 };
// Alpha solid (hover)
inline constexpr Color PlumA10{ 127, 0, 146, 187 };
// Alpha text
inline constexpr Color PlumA11{ 115, 0, 134, 193 };
// Alpha text (high contrast)
inline constexpr Color PlumA12{ 64, 0, 75, 230 };

// Background
inline constexpr Color Purple1{ 254, 252, 254, 255 };
// Background (subtle)
inline constexpr Color Purple2{ 251, 247, 254, 255 };
// Interactive background
inline constexpr Color Purple3{ 247, 237, 254, 255 };
// Interactive background (hover)
inline constexpr Color Purple4{ 242, 226, 252, 255 };
// Interactive background (active)
inline constexpr Color Purple5{ 234, 213, 249, 255 };
// Border (subtle)
inline constexpr Color Purple6{ 224, 196, 244, 255 };
// Border
inline constexpr Color Purple7{ 209, 175, 236, 255 };
// Border (strong)
inline constexpr Color Purple8{ 190, 147, 228, 255 };
// Solid
inline constexpr Color Purple9{ 142, 78, 198, 255 };
// Solid (hover)
inline constexpr Color Purple10{ 131, 71, 185, 255 };
// Text
inline constexpr Color Purple11{ 129, 69, 181, 255 };
// Text (high contrast)
inline constexpr Color Purple12{ 64, 32, 96, 255 };

// Alpha background
inline constexpr Color PurpleA1{ 170, 0, 170, 3 };
// Alpha background (subtle)
inline constexpr Color PurpleA2{ 128, 0, 224, 8 };
// Alpha interactive background
inline constexpr Color PurpleA3{ 142, 0, 241, 18 };
// Alpha interactive background (hover)
inline constexpr Color PurpleA4{ 141, 0, 229, 29 };
// Alpha interactive background (active)
inline constexpr Color PurpleA5{ 128, 0, 219, 42 };
// Alpha border (subtle)
inline constexpr Color PurpleA6{ 122, 1, 208, 59 };
// Alpha border
inline constexpr Color PurpleA7{ 109, 0, 195, 80 };
// Alpha border (strong)
inline constexpr Color PurpleA8{ 102, 0, 192, 108 };
// Alpha solid
inline constexpr Color PurpleA9{ 92, 0, 173, 177 };
// Alpha solid (hover)
inline constexpr Color PurpleA10{ 83, 0, 158, 184 };
// Alpha text
inline constexpr Color PurpleA11{ 82, 0, 154, 186 };
// Alpha text (high contrast)
inline constexpr Color PurpleA12{ 37, 0, 73, 223 };

// Background
inline constexpr Color Violet1{ 253, 252, 254, 255 };
// Background (subtle)
inline constexpr Color Violet2{ 250, 248, 255, 255 };
// Interactive background
inline constexpr Color Violet3{ 244, 240, 254, 255 };
// Interactive background (hover)
inline constexpr Color Violet4{ 235, 228, 255, 255 };
// Interactive background (active)
inline constexpr Color Violet5{ 225, 217, 255, 255 };
// Border (subtle)
inline constexpr Color Violet6{ 212, 202, 254, 255 };
// Border
inline constexpr Color Violet7{ 194, 181, 245, 255 };
// Border (strong)
inline constexpr Color Violet8{ 170, 153, 236, 255 };
// Solid
inline constexpr Color Violet9{ 110, 86, 207, 255 };
// Solid (hover)
inline constexpr Color Violet10{ 101, 77, 196, 255 };
// Text
inline constexpr Color Violet11{ 101, 80, 185, 255 };
// Text (high contrast)
inline constexpr Color Violet12{ 47, 38, 95, 255 };

// Alpha background
inline constexpr Color VioletA1{ 85, 0, 170, 3 };
// Alpha background (subtle)
inline constexpr Color VioletA2{ 73, 0, 255, 7 };
// Alpha interactive background
inline constexpr Color VioletA3{ 68, 0, 238, 15 };
// Alpha interactive background (hover)
inline constexpr Color VioletA4{ 67, 0, 255, 27 };
// Alpha interactive background (active)
inline constexpr Color VioletA5{ 54, 0, 255, 38 };
// Alpha border (subtle)
inline constexpr Color VioletA6{ 49, 0, 251, 53 };
// Alpha border
inline constexpr Color VioletA7{ 45, 1, 221, 74 };
// Alpha border (strong)
inline constexpr Color VioletA8{ 43, 0, 208, 102 };
// Alpha solid
inline constexpr Color VioletA9{ 36, 0, 183, 169 };
// Alpha solid (hover)
inline constexpr Color VioletA10{ 35, 0, 171, 178 };
// Alpha text
inline constexpr Color VioletA11{ 31, 0, 153, 175 };
// Alpha text (high contrast)
inline constexpr Color VioletA12{ 11, 0, 67, 217 };

// Background
inline constexpr Color Iris1{ 253, 253, 255, 255 };
// Background (subtle)
inline constexpr Color Iris2{ 248, 248, 255, 255 };
// Interactive background
inline constexpr Color Iris3{ 240, 241, 254, 255 };
// Interactive background (hover)
inline constexpr Color Iris4{ 230, 231, 255, 255 };
// Interactive background (active)
inline constexpr Color Iris5{ 218, 220, 255, 255 };
// Border (subtle)
inline constexpr Color Iris6{ 203, 205, 255, 255 };
// Border
inline constexpr Color Iris7{ 184, 186, 248, 255 };
// Border (strong)
inline constexpr Color Iris8{ 155, 158, 240, 255 };
// Solid
inline constexpr Color Iris9{ 91, 91, 214, 255 };
// Solid (hover)
inline constexpr Color Iris10{ 81, 81, 205, 255 };
// Text
inline constexpr Color Iris11{ 87, 83, 198, 255 };
// Text (high contrast)
inline constexpr Color Iris12{ 39, 41, 98, 255 };

// Alpha background
inline constexpr Color IrisA1{ 0, 0, 255, 2 };
// Alpha background (subtle)
inline constexpr Color IrisA2{ 0, 0, 255, 7 };
// Alpha interactive background
inline constexpr Color IrisA3{ 0, 17, 238, 15 };
// Alpha interactive background (hover)
inline constexpr Color IrisA4{ 0, 11, 255, 25 };
// Alpha interactive background (active)
inline constexpr Color IrisA5{ 0, 14, 255, 37 };
// Alpha border (subtle)
inline constexpr Color IrisA6{ 0, 10, 255, 52 };
// Alpha border
inline constexpr Color IrisA7{ 0, 8, 230, 71 };
// Alpha border (strong)
inline constexpr Color IrisA8{ 0, 8, 217, 100 };
// Alpha solid
inline constexpr Color IrisA9{ 0, 0, 192, 164 };
// Alpha solid (hover)
inline constexpr Color IrisA10{ 0, 0, 182, 174 };
// Alpha text
inline constexpr Color IrisA11{ 6, 0, 171, 172 };
// Alpha text (high contrast)
inline constexpr Color IrisA12{ 0, 2, 70, 216 };

// Background
inline constexpr Color Indigo1{ 253, 253, 254, 255 };
// Background (subtle)
inline constexpr Color Indigo2{ 247, 249, 255, 255 };
// Interactive background
inline constexpr Color Indigo3{ 237, 242, 254, 255 };
// Interactive background (hover)
inline constexpr Color Indigo4{ 225, 233, 255, 255 };
// Interactive background (active)
inline constexpr Color Indigo5{ 210, 222, 255, 255 };
// Border (subtle)
inline constexpr Color Indigo6{ 193, 208, 255, 255 };
// Border
inline constexpr Color Indigo7{ 171, 189, 249, 255 };
// Border (strong)
inline constexpr Color Indigo8{ 141, 164, 239, 255 };
// Solid
inline constexpr Color Indigo9{ 62, 99, 221, 255 };
// Solid (hover)
inline constexpr Color Indigo10{ 51, 88, 212, 255 };
// Text
inline constexpr Color Indigo11{ 58, 91, 199, 255 };
// Text (high contrast)
inline constexpr Color Indigo12{ 31, 45, 92, 255 };

// Alpha background
inline constexpr Color IndigoA1{ 0, 0, 128, 2 };
// Alpha background (subtle)
inline constexpr Color IndigoA2{ 0, 64, 255, 8 };
// Alpha interactive background
inline constexpr Color IndigoA3{ 0, 71, 241, 18 };
// Alpha interactive background (hover)
inline constexpr Color IndigoA4{ 0, 68, 255, 30 };
// Alpha interactive background (active)
inline constexpr Color IndigoA5{ 0, 68, 255, 45 };
// Alpha border (subtle)
inline constexpr Color IndigoA6{ 0, 62, 255, 62 };
// Alpha border
inline constexpr Color IndigoA7{ 0, 55, 237, 84 };
// Alpha border (strong)
inline constexpr Color IndigoA8{ 0, 52, 220, 114 };
// Alpha solid
inline constexpr Color IndigoA9{ 0, 49, 210, 193 };
// Alpha solid (hover)
inline constexpr Color IndigoA10{ 0, 46, 201, 204 };
// Alpha text
inline constexpr Color IndigoA11{ 0, 43, 183, 197 };
// Alpha text (high contrast)
inline constexpr Color IndigoA12{ 0, 16, 70, 224 };

// Background
inline constexpr Color Blue1{ 251, 253, 255, 255 };
// Background (subtle)
inline constexpr Color Blue2{ 244, 250, 255, 255 };
// Interactive background
inline constexpr Color Blue3{ 230, 244, 254, 255 };
// Interactive background (hover)
inline constexpr Color Blue4{ 213, 239, 255, 255 };
// Interactive background (active)
inline constexpr Color Blue5{ 194, 229, 255, 255 };
// Border (subtle)
inline constexpr Color Blue6{ 172, 216, 252, 255 };
// Border
inline constexpr Color Blue7{ 142, 200, 246, 255 };
// Border (strong)
inline constexpr Color Blue8{ 94, 177, 239, 255 };
// Solid
inline constexpr Color Blue9{ 0, 144, 255, 255 };
// Solid (hover)
inline constexpr Color Blue10{ 5, 136, 240, 255 };
// Text
inline constexpr Color Blue11{ 13, 116, 206, 255 };
// Text (high contrast)
inline constexpr Color Blue12{ 17, 50, 100, 255 };

// Alpha background
inline constexpr Color BlueA1{ 0, 128, 255, 4 };
// Alpha background (subtle)
inline constexpr Color BlueA2{ 0, 140, 255, 11 };
// Alpha interactive background
inline constexpr Color BlueA3{ 0, 143, 245, 25 };
// Alpha interactive background (hover)
inline constexpr Color BlueA4{ 0, 158, 255, 42 };
// Alpha interactive background (active)
inline constexpr Color BlueA5{ 0, 147, 255, 61 };
// Alpha border (subtle)
inline constexpr Color BlueA6{ 0, 136, 246, 83 };
// Alpha border
inline constexpr Color BlueA7{ 0, 131, 235, 113 };
// Alpha border (strong)
inline constexpr Color BlueA8{ 0, 132, 230, 161 };
// Alpha solid
inline constexpr Color BlueA9{ 0, 144, 255, 255 };
// Alpha solid (hover)
inline constexpr Color BlueA10{ 0, 134, 240, 250 };
// Alpha text
inline constexpr Color BlueA11{ 0, 109, 203, 242 };
// Alpha text (high contrast)
inline constexpr Color BlueA12{ 0, 35, 89, 238 };

// Background
inline constexpr Color Cyan1{ 250, 253, 254, 255 };
// Background (subtle)
inline constexpr Color Cyan2{ 242, 250, 251, 255 };
// Interactive background
inline constexpr Color Cyan3{ 222, 247, 249, 255 };
// Interactive background (hover)
inline constexpr Color Cyan4{ 202, 241, 246, 255 };
// Interactive background (active)
inline constexpr Color Cyan5{ 181, 233, 240, 255 };
// Border (subtle)
inline constexpr Color Cyan6{ 157, 221, 231, 255 };
// Border
inline constexpr Color Cyan7{ 125, 206, 220, 255 };
// Border (strong)
inline constexpr Color Cyan8{ 61, 185, 207, 255 };
// Solid
inline constexpr Color Cyan9{ 0, 162, 199, 255 };
// Solid (hover)
inline constexpr Color Cyan10{ 7, 151, 185, 255 };
// Text
inline constexpr Color Cyan11{ 16, 125, 152, 255 };
// Text (high contrast)
inline constexpr Color Cyan12{ 13, 60, 72, 255 };

// Alpha background
inline constexpr Color CyanA1{ 0, 153, 204, 5 };
// Alpha background (subtle)
inline constexpr Color CyanA2{ 0, 157, 177, 13 };
// Alpha interactive background
inline constexpr Color CyanA3{ 0, 194, 209, 33 };
// Alpha interactive background (hover)
inline constexpr Color CyanA4{ 0, 188, 212, 53 };
// Alpha interactive background (active)
inline constexpr Color CyanA5{ 1, 180, 204, 74 };
// Alpha border (subtle)
inline constexpr Color CyanA6{ 0, 167, 193, 98 };
// Alpha border
inline constexpr Color CyanA7{ 0, 159, 187, 130 };
// Alpha border (strong)
inline constexpr Color CyanA8{ 0, 163, 192, 194 };
// Alpha solid
inline constexpr Color CyanA9{ 0, 162, 199, 255 };
// Alpha solid (hover)
inline constexpr Color CyanA10{ 0, 148, 183, 248 };
// Alpha text
inline constexpr Color CyanA11{ 0, 116, 145, 239 };
// Alpha text (high contrast)
inline constexpr Color CyanA12{ 0, 50, 62, 242 };

// Background
inline constexpr Color Sky1{ 249, 254, 255, 255 };
// Background (subtle)
inline constexpr Color Sky2{ 241, 250, 253, 255 };
// Interactive background
inline constexpr Color Sky3{ 225, 246, 253, 255 };
// Interactive background (hover)
inline constexpr Color Sky4{ 209, 240, 250, 255 };
// Interactive background (active)
inline constexpr Color Sky5{ 190, 231, 245, 255 };
// Border (subtle)
inline constexpr Color Sky6{ 169, 218, 237, 255 };
// Border
inline constexpr Color Sky7{ 141, 202, 227, 255 };
// Border (strong)
inline constexpr Color Sky8{ 96, 179, 215, 255 };
// Solid
inline constexpr Color Sky9{ 124, 226, 254, 255 };
// Solid (hover)
inline constexpr Color Sky10{ 116, 218, 248, 255 };
// Text
inline constexpr Color Sky11{ 0, 116, 158, 255 };
// Text (high contrast)
inline constexpr Color Sky12{ 29, 62, 86, 255 };

// Alpha background
inline constexpr Color SkyA1{ 0, 213, 255, 6 };
// Alpha background (subtle)
inline constexpr Color SkyA2{ 0, 164, 219, 14 };
// Alpha interactive background
inline constexpr Color SkyA3{ 0, 179, 238, 30 };
// Alpha interactive background (hover)
inline constexpr Color SkyA4{ 0, 172, 228, 46 };
// Alpha interactive background (active)
inline constexpr Color SkyA5{ 0, 161, 216, 65 };
// Alpha border (subtle)
inline constexpr Color SkyA6{ 0, 146, 202, 86 };
// Alpha border
inline constexpr Color SkyA7{ 0, 137, 193, 114 };
// Alpha border (strong)
inline constexpr Color SkyA8{ 0, 133, 191, 159 };
// Alpha solid
inline constexpr Color SkyA9{ 0, 199, 254, 131 };
// Alpha solid (hover)
inline constexpr Color SkyA10{ 0, 188, 243, 139 };
// Alpha text
inline constexpr Color SkyA11{ 0, 116, 158, 255 };
// Alpha text (high contrast)
inline constexpr Color SkyA12{ 0, 37, 64, 226 };

// Background
inline constexpr Color Teal1{ 250, 254, 253, 255 };
// Background (subtle)
inline constexpr Color Teal2{ 243, 251, 249, 255 };
// Interactive background
inline constexpr Color Teal3{ 224, 248, 243, 255 };
// Interactive background (hover)
inline constexpr Color Teal4{ 204, 243, 234, 255 };
// Interactive background (active)
inline constexpr Color Teal5{ 184, 234, 224, 255 };
// Border (subtle)
inline constexpr Color Teal6{ 161, 222, 210, 255 };
// Border
inline constexpr Color Teal7{ 131, 205, 193, 255 };
// Border (strong)
inline constexpr Color Teal8{ 83, 185, 171, 255 };
// Solid
inline constexpr Color Teal9{ 18, 165, 148, 255 };
// Solid (hover)
inline constexpr Color Teal10{ 13, 155, 138, 255 };
// Text
inline constexpr Color Teal11{ 0, 133, 115, 255 };
// Text (high contrast)
inline constexpr Color Teal12{ 13, 61, 56, 255 };

// Alpha background
inline constexpr Color TealA1{ 0, 204, 153, 5 };
// Alpha background (subtle)
inline constexpr Color TealA2{ 0, 170, 128, 12 };
// Alpha interactive background
inline constexpr Color TealA3{ 0, 198, 157, 31 };
// Alpha interactive background (hover)
inline constexpr Color TealA4{ 0, 195, 150, 51 };
// Alpha interactive background (active)
inline constexpr Color TealA5{ 0, 180, 144, 71 };
// Alpha border (subtle)
inline constexpr Color TealA6{ 0, 166, 133, 94 };
// Alpha border
inline constexpr Color TealA7{ 0, 153, 128, 124 };
// Alpha border (strong)
inline constexpr Color TealA8{ 0, 151, 131, 172 };
// Alpha solid
inline constexpr Color TealA9{ 0, 158, 140, 237 };
// Alpha solid (hover)
inline constexpr Color TealA10{ 0, 150, 132, 242 };
// Alpha text
inline constexpr Color TealA11{ 0, 133, 115, 255 };
// Alpha text (high contrast)
inline constexpr Color TealA12{ 0, 51, 45, 242 };

// Background
inline constexpr Color Jade1{ 251, 254, 253, 255 };
// Background (subtle)
inline constexpr Color Jade2{ 244, 251, 247, 255 };
// Interactive background
inline constexpr Color Jade3{ 230, 247, 237, 255 };
// Interactive background (hover)
inline constexpr Color Jade4{ 214, 241, 227, 255 };
// Interactive background (active)
inline constexpr Color Jade5{ 195, 233, 215, 255 };
// Border (subtle)
inline constexpr Color Jade6{ 172, 222, 200, 255 };
// Border
inline constexpr Color Jade7{ 139, 206, 182, 255 };
// Border (strong)
inline constexpr Color Jade8{ 86, 186, 159, 255 };
// Solid
inline constexpr Color Jade9{ 41, 163, 131, 255 };
// Solid (hover)
inline constexpr Color Jade10{ 38, 153, 123, 255 };
// Text
inline constexpr Color Jade11{ 32, 131, 104, 255 };
// Text (high contrast)
inline constexpr Color Jade12{ 29, 59, 49, 255 };

// Alpha background
inline constexpr Color JadeA1{ 0, 192, 128, 4 };
// Alpha background (subtle)
inline constexpr Color JadeA2{ 0, 163, 70, 11 };
// Alpha interactive background
inline constexpr Color JadeA3{ 0, 174, 72, 25 };
// Alpha interactive background (hover)
inline constexpr Color JadeA4{ 0, 168, 81, 41 };
// Alpha interactive background (active)
inline constexpr Color JadeA5{ 0, 162, 85, 60 };
// Alpha border (subtle)
inline constexpr Color JadeA6{ 0, 154, 87, 83 };
// Alpha border
inline constexpr Color JadeA7{ 0, 148, 95, 116 };
// Alpha border (strong)
inline constexpr Color JadeA8{ 0, 151, 110, 169 };
// Alpha solid
inline constexpr Color JadeA9{ 0, 145, 107, 214 };
// Alpha solid (hover)
inline constexpr Color JadeA10{ 0, 135, 100, 217 };
// Alpha text
inline constexpr Color JadeA11{ 0, 113, 82, 223 };
// Alpha text (high contrast)
inline constexpr Color JadeA12{ 0, 34, 23, 226 };

// Background
inline constexpr Color Green1{ 251, 254, 252, 255 };
// Background (subtle)
inline constexpr Color Green2{ 244, 251, 246, 255 };
// Interactive background
inline constexpr Color Green3{ 230, 246, 235, 255 };
// Interactive background (hover)
inline constexpr Color Green4{ 214, 241, 223, 255 };
// Interactive background (active)
inline constexpr Color Green5{ 196, 232, 209, 255 };
// Border (subtle)
inline constexpr Color Green6{ 173, 221, 192, 255 };
// Border
inline constexpr Color Green7{ 142, 206, 170, 255 };
// Border (strong)
inline constexpr Color Green8{ 91, 185, 139, 255 };
// Solid
inline constexpr Color Green9{ 48, 164, 108, 255 };
// Solid (hover)
inline constexpr Color Green10{ 43, 154, 102, 255 };
// Text
inline constexpr Color Green11{ 33, 131, 88, 255 };
// Text (high contrast)
inline constexpr Color Green12{ 25, 59, 45, 255 };

// Alpha background
inline constexpr Color GreenA1{ 0, 192, 64, 4 };
// Alpha background (subtle)
inline constexpr Color GreenA2{ 0, 163, 47, 11 };
// Alpha interactive background
inline constexpr Color GreenA3{ 0, 164, 51, 25 };
// Alpha interactive background (hover)
inline constexpr Color GreenA4{ 0, 168, 56, 41 };
// Alpha interactive background (active)
inline constexpr Color GreenA5{ 1, 156, 57, 59 };
// Alpha border (subtle)
inline constexpr Color GreenA6{ 0, 150, 60, 82 };
// Alpha border
inline constexpr Color GreenA7{ 0, 145, 64, 113 };
// Alpha border (strong)
inline constexpr Color GreenA8{ 0, 146, 75, 164 };
// Alpha solid
inline constexpr Color GreenA9{ 0, 143, 74, 207 };
// Alpha solid (hover)
inline constexpr Color GreenA10{ 0, 134, 71, 212 };
// Alpha text
inline constexpr Color GreenA11{ 0, 113, 63, 222 };
// Alpha text (high contrast)
inline constexpr Color GreenA12{ 0, 38, 22, 230 };

// Background
inline constexpr Color Grass1{ 251, 254, 251, 255 };
// Background (subtle)
inline constexpr Color Grass2{ 245, 251, 245, 255 };
// Interactive background
inline constexpr Color Grass3{ 233, 246, 233, 255 };
// Interactive background (hover)
inline constexpr Color Grass4{ 218, 241, 219, 255 };
// Interactive background (active)
inline constexpr Color Grass5{ 201, 232, 202, 255 };
// Border (subtle)
inline constexpr Color Grass6{ 178, 221, 181, 255 };
// Border
inline constexpr Color Grass7{ 148, 206, 154, 255 };
// Border (strong)
inline constexpr Color Grass8{ 101, 186, 116, 255 };
// Solid
inline constexpr Color Grass9{ 70, 167, 88, 255 };
// Solid (hover)
inline constexpr Color Grass10{ 62, 155, 79, 255 };
// Text
inline constexpr Color Grass11{ 42, 126, 59, 255 };
// Text (high contrast)
inline constexpr Color Grass12{ 32, 60, 37, 255 };

// Alpha background
inline constexpr Color GrassA1{ 0, 192, 0, 4 };
// Alpha background (subtle)
inline constexpr Color GrassA2{ 0, 153, 0, 10 };
// Alpha interactive background
inline constexpr Color GrassA3{ 0, 151, 0, 22 };
// Alpha interactive background (hover)
inline constexpr Color GrassA4{ 0, 159, 7, 37 };
// Alpha interactive background (active)
inline constexpr Color GrassA5{ 0, 147, 5, 54 };
// Alpha border (subtle)
inline constexpr Color GrassA6{ 0, 143, 10, 77 };
// Alpha border
inline constexpr Color GrassA7{ 1, 139, 15, 107 };
// Alpha border (strong)
inline constexpr Color GrassA8{ 0, 141, 25, 154 };
// Alpha solid
inline constexpr Color GrassA9{ 0, 134, 25, 185 };
// Alpha solid (hover)
inline constexpr Color GrassA10{ 0, 123, 23, 193 };
// Alpha text
inline constexpr Color GrassA11{ 0, 101, 20, 213 };
// Alpha text (high contrast)
inline constexpr Color GrassA12{ 0, 32, 6, 223 };

// Background
inline constexpr Color Mint1{ 249, 254, 253, 255 };
// Background (subtle)
inline constexpr Color Mint2{ 242, 251, 249, 255 };
// Interactive background
inline constexpr Color Mint3{ 221, 249, 242, 255 };
// Interactive background (hover)
inline constexpr Color Mint4{ 200, 244, 233, 255 };
// Interactive background (active)
inline constexpr Color Mint5{ 179, 236, 222, 255 };
// Border (subtle)
inline constexpr Color Mint6{ 156, 224, 208, 255 };
// Border
inline constexpr Color Mint7{ 126, 207, 189, 255 };
// Border (strong)
inline constexpr Color Mint8{ 76, 187, 165, 255 };
// Solid
inline constexpr Color Mint9{ 134, 234, 212, 255 };
// Solid (hover)
inline constexpr Color Mint10{ 125, 224, 203, 255 };
// Text
inline constexpr Color Mint11{ 2, 120, 100, 255 };
// Text (high contrast)
inline constexpr Color Mint12{ 22, 67, 60, 255 };

// Alpha background
inline constexpr Color MintA1{ 0, 213, 170, 6 };
// Alpha background (subtle)
inline constexpr Color MintA2{ 0, 177, 138, 13 };
// Alpha interactive background
inline constexpr Color MintA3{ 0, 210, 158, 34 };
// Alpha interactive background (hover)
inline constexpr Color MintA4{ 0, 204, 153, 55 };
// Alpha interactive background (active)
inline constexpr Color MintA5{ 0, 192, 145, 76 };
// Alpha border (subtle)
inline constexpr Color MintA6{ 0, 176, 134, 99 };
// Alpha border
inline constexpr Color MintA7{ 0, 161, 125, 129 };
// Alpha border (strong)
inline constexpr Color MintA8{ 0, 158, 127, 179 };
// Alpha solid
inline constexpr Color MintA9{ 0, 211, 165, 121 };
// Alpha solid (hover)
inline constexpr Color MintA10{ 0, 195, 153, 130 };
// Alpha text
inline constexpr Color MintA11{ 0, 119, 99, 253 };
// Alpha text (high contrast)
inline constexpr Color MintA12{ 0, 49, 42, 233 };

// Background
inline constexpr Color Lime1{ 252, 253, 250, 255 };
// Background (subtle)
inline constexpr Color Lime2{ 248, 250, 243, 255 };
// Interactive background
inline constexpr Color Lime3{ 238, 246, 214, 255 };
// Interactive background (hover)
inline constexpr Color Lime4{ 226, 240, 189, 255 };
// Interactive background (active)
inline constexpr Color Lime5{ 211, 231, 166, 255 };
// Border (subtle)
inline constexpr Color Lime6{ 194, 218, 145, 255 };
// Border
inline constexpr Color Lime7{ 171, 201, 120, 255 };
// Border (strong)
inline constexpr Color Lime8{ 141, 182, 84, 255 };
// Solid
inline constexpr Color Lime9{ 189, 238, 99, 255 };
// Solid (hover)
inline constexpr Color Lime10{ 176, 230, 76, 255 };
// Text
inline constexpr Color Lime11{ 92, 124, 47, 255 };
// Text (high contrast)
inline constexpr Color Lime12{ 55, 64, 28, 255 };

// Alpha background
inline constexpr Color LimeA1{ 102, 153, 0, 5 };
// Alpha background (subtle)
inline constexpr Color LimeA2{ 107, 149, 0, 12 };
// Alpha interactive background
inline constexpr Color LimeA3{ 150, 200, 0, 41 };
// Alpha interactive background (hover)
inline constexpr Color LimeA4{ 143, 198, 0, 66 };
// Alpha interactive background (active)
inline constexpr Color LimeA5{ 129, 187, 0, 89 };
// Alpha border (subtle)
inline constexpr Color LimeA6{ 114, 170, 0, 110 };
// Alpha border
inline constexpr Color LimeA7{ 97, 153, 0, 135 };
// Alpha border (strong)
inline constexpr Color LimeA8{ 85, 146, 0, 171 };
// Alpha solid
inline constexpr Color LimeA9{ 147, 228, 0, 156 };
// Alpha solid (hover)
inline constexpr Color LimeA10{ 143, 220, 0, 179 };
// Alpha text
inline constexpr Color LimeA11{ 55, 95, 0, 208 };
// Alpha text (high contrast)
inline constexpr Color LimeA12{ 30, 41, 0, 227 };

// Background
inline constexpr Color Yellow1{ 253, 253, 249, 255 };
// Background (subtle)
inline constexpr Color Yellow2{ 254, 252, 233, 255 };
// Interactive background
inline constexpr Color Yellow3{ 255, 250, 184, 255 };
// Interactive background (hover)
inline constexpr Color Yellow4{ 255, 243, 148, 255 };
// Interactive background (active)
inline constexpr Color Yellow5{ 255, 231, 112, 255 };
// Border (subtle)
inline constexpr Color Yellow6{ 243, 215, 104, 255 };
// Border
inline constexpr Color Yellow7{ 228, 199, 103, 255 };
// Border (strong)
inline constexpr Color Yellow8{ 213, 174, 57, 255 };
// Solid
inline constexpr Color Yellow9{ 255, 230, 41, 255 };
// Solid (hover)
inline constexpr Color Yellow10{ 255, 220, 0, 255 };
// Text
inline constexpr Color Yellow11{ 158, 108, 0, 255 };
// Text (high contrast)
inline constexpr Color Yellow12{ 71, 59, 31, 255 };

// Alpha background
inline constexpr Color YellowA1{ 170, 170, 0, 6 };
// Alpha background (subtle)
inline constexpr Color YellowA2{ 244, 221, 0, 22 };
// Alpha interactive background
inline constexpr Color YellowA3{ 255, 238, 0, 71 };
// Alpha interactive background (hover)
inline constexpr Color YellowA4{ 255, 227, 1, 107 };
// Alpha interactive background (active)
inline constexpr Color YellowA5{ 255, 213, 0, 143 };
// Alpha border (subtle)
inline constexpr Color YellowA6{ 235, 188, 0, 151 };
// Alpha border
inline constexpr Color YellowA7{ 210, 161, 0, 152 };
// Alpha border (strong)
inline constexpr Color YellowA8{ 201, 151, 0, 198 };
// Alpha solid
inline constexpr Color YellowA9{ 255, 225, 0, 214 };
// Alpha solid (hover)
inline constexpr Color YellowA10{ 255, 220, 0, 255 };
// Alpha text
inline constexpr Color YellowA11{ 158, 108, 0, 255 };
// Alpha text (high contrast)
inline constexpr Color YellowA12{ 46, 32, 0, 224 };

// Background
inline constexpr Color Amber1{ 254, 253, 251, 255 };
// Background (subtle)
inline constexpr Color Amber2{ 254, 251, 233, 255 };
// Interactive background
inline constexpr Color Amber3{ 255, 247, 194, 255 };
// Interactive background (hover)
inline constexpr Color Amber4{ 255, 238, 156, 255 };
// Interactive background (active)
inline constexpr Color Amber5{ 251, 229, 119, 255 };
// Border (subtle)
inline constexpr Color Amber6{ 243, 214, 115, 255 };
// Border
inline constexpr Color Amber7{ 233, 193, 98, 255 };
// Border (strong)
inline constexpr Color Amber8{ 226, 163, 54, 255 };
// Solid
inline constexpr Color Amber9{ 255, 197, 61, 255 };
// Solid (hover)
inline constexpr Color Amber10{ 255, 186, 24, 255 };
// Text
inline constexpr Color Amber11{ 171, 100, 0, 255 };
// Text (high contrast)
inline constexpr Color Amber12{ 79, 52, 34, 255 };

// Alpha background
inline constexpr Color AmberA1{ 192, 128, 0, 4 };
// Alpha background (subtle)
inline constexpr Color AmberA2{ 244, 209, 0, 22 };
// Alpha interactive background
inline constexpr Color AmberA3{ 255, 222, 0, 61 };
// Alpha interactive background (hover)
inline constexpr Color AmberA4{ 255, 212, 0, 99 };
// Alpha interactive background (active)
inline constexpr Color AmberA5{ 248, 207, 0, 136 };
// Alpha border (subtle)
inline constexpr Color AmberA6{ 234, 181, 0, 140 };
// Alpha border
inline constexpr Color AmberA7{ 220, 155, 0, 157 };
// Alpha border (strong)
inline constexpr Color AmberA8{ 218, 138, 0, 201 };
// Alpha solid
inline constexpr Color AmberA9{ 255, 179, 0, 194 };
// Alpha solid (hover)
inline constexpr Color AmberA10{ 255, 179, 0, 231 };
// Alpha text
inline constexpr Color AmberA11{ 171, 100, 0, 255 };
// Alpha text (high contrast)
inline constexpr Color AmberA12{ 52, 21, 0, 221 };

// Background
inline constexpr Color Orange1{ 254, 252, 251, 255 };
// Background (subtle)
inline constexpr Color Orange2{ 255, 247, 237, 255 };
// Interactive background
inline constexpr Color Orange3{ 255, 239, 214, 255 };
// Interactive background (hover)
inline constexpr Color Orange4{ 255, 223, 181, 255 };
// Interactive background (active)
inline constexpr Color Orange5{ 255, 209, 154, 255 };
// Border (subtle)
inline constexpr Color Orange6{ 255, 193, 130, 255 };
// Border
inline constexpr Color Orange7{ 245, 174, 115, 255 };
// Border (strong)
inline constexpr Color Orange8{ 236, 148, 85, 255 };
// Solid
inline constexpr Color Orange9{ 247, 107, 21, 255 };
// Solid (hover)
inline constexpr Color Orange10{ 239, 95, 0, 255 };
// Text
inline constexpr Color Orange11{ 204, 78, 0, 255 };
// Text (high contrast)
inline constexpr Color Orange12{ 88, 45, 29, 255 };

// Alpha background
inline constexpr Color OrangeA1{ 192, 64, 0, 4 };
// Alpha background (subtle)
inline constexpr Color OrangeA2{ 255, 142, 0, 18 };
// Alpha interactive background
inline constexpr Color OrangeA3{ 255, 156, 0, 41 };
// Alpha interactive background (hover)
inline constexpr Color OrangeA4{ 255, 145, 1, 74 };
// Alpha interactive background (active)
inline constexpr Color OrangeA5{ 255, 139, 0, 101 };
// Alpha border (subtle)
inline constexpr Color OrangeA6{ 255, 129, 0, 125 };
// Alpha border
inline constexpr Color OrangeA7{ 237, 108, 0, 140 };
// Alpha border (strong)
inline constexpr Color OrangeA8{ 227, 95, 0, 170 };
// Alpha solid
inline constexpr Color OrangeA9{ 246, 94, 0, 234 };
// Alpha solid (hover)
inline constexpr Color OrangeA10{ 239, 95, 0, 255 };
// Alpha text
inline constexpr Color OrangeA11{ 204, 78, 0, 255 };
// Alpha text (high contrast)
inline constexpr Color OrangeA12{ 67, 18, 0, 226 };

// Background
inline constexpr Color Brown1{ 254, 253, 252, 255 };
// Background (subtle)
inline constexpr Color Brown2{ 252, 249, 246, 255 };
// Interactive background
inline constexpr Color Brown3{ 246, 238, 231, 255 };
// Interactive background (hover)
inline constexpr Color Brown4{ 240, 228, 217, 255 };
// Interactive background (active)
inline constexpr Color Brown5{ 235, 218, 202, 255 };
// Border (subtle)
inline constexpr Color Brown6{ 228, 205, 183, 255 };
// Border
inline constexpr Color Brown7{ 220, 188, 159, 255 };
// Border (strong)
inline constexpr Color Brown8{ 206, 163, 126, 255 };
// Solid
inline constexpr Color Brown9{ 173, 127, 88, 255 };
// Solid (hover)
inline constexpr Color Brown10{ 160, 117, 83, 255 };
// Text
inline constexpr Color Brown11{ 129, 94, 70, 255 };
// Text (high contrast)
inline constexpr Color Brown12{ 62, 51, 46, 255 };

// Alpha background
inline constexpr Color BrownA1{ 170, 85, 0, 3 };
// Alpha background (subtle)
inline constexpr Color BrownA2{ 170, 85, 0, 9 };
// Alpha interactive background
inline constexpr Color BrownA3{ 160, 75, 0, 24 };
// Alpha interactive background (hover)
inline constexpr Color BrownA4{ 155, 74, 0, 38 };
// Alpha interactive background (active)
inline constexpr Color BrownA5{ 159, 77, 0, 53 };
// Alpha border (subtle)
inline constexpr Color BrownA6{ 160, 78, 0, 72 };
// Alpha border
inline constexpr Color BrownA7{ 163, 78, 0, 96 };
// Alpha border (strong)
inline constexpr Color BrownA8{ 159, 74, 0, 129 };
// Alpha solid
inline constexpr Color BrownA9{ 130, 60, 0, 167 };
// Alpha solid (hover)
inline constexpr Color BrownA10{ 114, 51, 0, 172 };
// Alpha text
inline constexpr Color BrownA11{ 82, 33, 0, 185 };
// Alpha text (high contrast)
inline constexpr Color BrownA12{ 20, 6, 0, 209 };

// Background
inline constexpr Color Bronze1{ 253, 252, 252, 255 };
// Background (subtle)
inline constexpr Color Bronze2{ 253, 247, 245, 255 };
// Interactive background
inline constexpr Color Bronze3{ 246, 237, 234, 255 };
// Interactive background (hover)
inline constexpr Color Bronze4{ 239, 228, 223, 255 };
// Interactive background (active)
inline constexpr Color Bronze5{ 231, 217, 211, 255 };
// Border (subtle)
inline constexpr Color Bronze6{ 223, 205, 197, 255 };
// Border
inline constexpr Color Bronze7{ 211, 188, 179, 255 };
// Border (strong)
inline constexpr Color Bronze8{ 194, 164, 153, 255 };
// Solid
inline constexpr Color Bronze9{ 161, 128, 114, 255 };
// Solid (hover)
inline constexpr Color Bronze10{ 149, 116, 104, 255 };
// Text
inline constexpr Color Bronze11{ 125, 94, 84, 255 };
// Text (high contrast)
inline constexpr Color Bronze12{ 67, 48, 43, 255 };

// Alpha background
inline constexpr Color BronzeA1{ 85, 0, 0, 3 };
// Alpha background (subtle)
inline constexpr Color BronzeA2{ 204, 51, 0, 10 };
// Alpha interactive background
inline constexpr Color BronzeA3{ 146, 37, 0, 21 };
// Alpha interactive background (hover)
inline constexpr Color BronzeA4{ 128, 40, 0, 32 };
// Alpha interactive background (active)
inline constexpr Color BronzeA5{ 116, 35, 0, 44 };
// Alpha border (subtle)
inline constexpr Color BronzeA6{ 115, 36, 0, 58 };
// Alpha border
inline constexpr Color BronzeA7{ 108, 31, 0, 76 };
// Alpha border (strong)
inline constexpr Color BronzeA8{ 103, 28, 0, 102 };
// Alpha solid
inline constexpr Color BronzeA9{ 85, 26, 0, 141 };
// Alpha solid (hover)
inline constexpr Color BronzeA10{ 76, 21, 0, 151 };
// Alpha text
inline constexpr Color BronzeA11{ 61, 15, 0, 171 };
// Alpha text (high contrast)
inline constexpr Color BronzeA12{ 29, 6, 0, 212 };

// Background
inline constexpr Color Gold1{ 253, 253, 252, 255 };
// Background (subtle)
inline constexpr Color Gold2{ 250, 249, 242, 255 };
// Interactive background
inline constexpr Color Gold3{ 242, 240, 231, 255 };
// Interactive background (hover)
inline constexpr Color Gold4{ 234, 230, 219, 255 };
// Interactive background (active)
inline constexpr Color Gold5{ 225, 220, 207, 255 };
// Border (subtle)
inline constexpr Color Gold6{ 216, 208, 191, 255 };
// Border
inline constexpr Color Gold7{ 203, 192, 170, 255 };
// Border (strong)
inline constexpr Color Gold8{ 185, 168, 141, 255 };
// Solid
inline constexpr Color Gold9{ 151, 131, 101, 255 };
// Solid (hover)
inline constexpr Color Gold10{ 140, 122, 94, 255 };
// Text
inline constexpr Color Gold11{ 113, 98, 75, 255 };
// Text (high contrast)
inline constexpr Color Gold12{ 59, 53, 43, 255 };

// Alpha background
inline constexpr Color GoldA1{ 85, 85, 0, 3 };
// Alpha background (subtle)
inline constexpr Color GoldA2{ 157, 138, 0, 13 };
// Alpha interactive background
inline constexpr Color GoldA3{ 117, 96, 0, 24 };
// Alpha interactive background (hover)
inline constexpr Color GoldA4{ 107, 78, 0, 36 };
// Alpha interactive background (active)
inline constexpr Color GoldA5{ 96, 70, 0, 48 };
// Alpha border (subtle)
inline constexpr Color GoldA6{ 100, 68, 0, 64 };
// Alpha border
inline constexpr Color GoldA7{ 99, 66, 0, 85 };
// Alpha border (strong)
inline constexpr Color GoldA8{ 99, 61, 0, 114 };
// Alpha solid
inline constexpr Color GoldA9{ 83, 50, 0, 154 };
// Alpha solid (hover)
inline constexpr Color GoldA10{ 73, 45, 0, 161 };
// Alpha text
inline constexpr Color GoldA11{ 54, 33, 0, 180 };
// Alpha text (high contrast)
inline constexpr Color GoldA12{ 19, 12, 0, 212 };
} // namespace onyx::colors::light

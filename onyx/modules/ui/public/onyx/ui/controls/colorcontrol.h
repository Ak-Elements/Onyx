#pragma once

namespace Onyx::Ui
{
    bool ColorInput(StringView id, Vector3u8& rgb);
    bool ColorInput(StringView id, Vector4u8& rgba);

    bool ColorInput(StringView id, Vector3f32& hsv);
    bool ColorInput(StringView id, Vector4f32& hsva);
}

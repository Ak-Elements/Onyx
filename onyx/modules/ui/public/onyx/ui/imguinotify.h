/**
 * @file ImGuiNotify.hpp
 * @brief A header-only library for creating toast notifications with ImGui.
 *
 * Based on imgui-notify by patrickcjk
 * https://github.com/patrickcjk/imgui-notify
 *
 * Based on imgui-notify by TyomaVader
 * https://github.com/TyomaVader/ImGuiNotify
 *
 */

#if ONYX_USE_IMGUI

#ifndef IMGUI_NOTIFY
#define IMGUI_NOTIFY

#pragma once

#include <vector>			// Vector for storing notifications list
#include <string>
#include <chrono>			// For the notifications timed dissmiss
#include <functional>		// For storing the code, which executest on the button click in the notification

#include <imgui.h>
#include <imgui_internal.h>

#include <onyx/ui/widgets.h>

static const ImGuiWindowFlags NOTIFY_DEFAULT_TOAST_FLAGS = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;

enum class ImGuiToastType : uint8_t
{
    None,
    Success,
    Warning,
    Error,
    Info,
    COUNT
};

enum class ImGuiToastPhase : uint8_t
{
    Queued,
    FadeIn,
    Wait,
    FadeOut,
    Expired,
    COUNT
};

enum class ImGuiToastPos : uint8_t
{
    TopLeft,
    TopCenter,
    TopRight,
    BottomLeft,
    BottomCenter,
    BottomRight,
    Center,
    COUNT
};

enum class ImGuiToastDirection : uint8_t
{
    Up,
    Down,
    COUNT
};

enum class ImGuiToastOrder : uint8_t
{
    FIFO, // First In First out
    LIFO, // Last In First out
    COUNT
};

/**
  * CONFIGURATION SECTION Start
 */

struct ImGuiNotifySettings
{
    uint32_t MaxMessageLength = 4096;                       // Max message content length
    float VerticalMessagePadding = 10.0f;                   // Padding Y between each message
    uint32_t FadeOutAnimationTimeInMilliseconds = 150;      // Fade in and out duration
    int32_t DefaultAutoDismissInMilliseconds = 3000;        // Auto dismiss after X ms (default, applied only of no data provided in constructors)
    float ToastAlpha = 0.8f;                                // 0-1 Toast alpha
    int32_t MaxNotifications = 5;                           // Max number of toasts rendered at the same time. Set to 0 for unlimited
    ImVec2 MinSize = { 300.0f, 100.0f };
    ImVec2 MaxSize = { 300.0f, 200.0f };
    bool RenderOnScreen = false;                            // Set to true to render on screen instead of inside main window - Needs ImGui Multiviewport 
    bool AutoDismissOnlyVisibleNotifications = true;

    ImGuiToastPos Position = ImGuiToastPos::TopRight;  // Position for the toast notifications on screen or main window
    ImGuiToastDirection Direction = ImGuiToastDirection::Down;  // Direction of the toast notifications if more than 1 are shown
    ImGuiToastOrder Order = ImGuiToastOrder::FIFO;
};

/**
 * @brief A class for creating toast notifications with ImGui.
 */
class ImGuiToast
{
private:
    ImGuiWindowFlags							m_Flags = NOTIFY_DEFAULT_TOAST_FLAGS;

    ImGuiToastType								m_Type = ImGuiToastType::None;
    ImGuiToastPhase                             m_State = ImGuiToastPhase::Queued;
    std::string									m_Title;
    std::string									m_Content;

    int32_t										m_DismissTime;
    uint64_t		                            m_CreationTime = Onyx::Time::GetCurrentMilliseconds();

    bool m_UseSeperator = false;
    bool m_HasCloseButton = false;

    std::function<bool()>						m_OnPress = nullptr; // A lambda variable, which will be executed when button in notification is pressed, return true if the notification should be removed
    std::function<bool()>						m_CustomContent = nullptr; // A lambda which can be used to render custom imgui content within the notification, return true if the notification should be removed

public:
    void SetTitle(std::string_view title) { m_Title = title; }
    void SetContent(std::string_view content) { m_Content = content; }

    void SetCreationTime(uint64_t timeStamp) { m_CreationTime = timeStamp; }

    void SetCustomContentFunctor(const std::function<bool()>& functor) { m_CustomContent = functor; }
    const std::function<bool()>& GetCustomContentFunctor() const { return m_CustomContent; }
    
    /**
     * @brief Set the type of the toast notification.
     *
     * @param type The type of the toast notification.
     */
    void SetType(const ImGuiToastType& type)
    {
        IM_ASSERT(type < ImGuiToastType::COUNT);
        m_Type = type;
    };

    /**
     * @brief Set the ImGui window flags for the notification.
     *
     * @param flags ImGui window flags to set.
    */
    inline void SetWindowFlags(const ImGuiWindowFlags& flags)
    {
        m_Flags = flags;
    }

    /**
     * @brief Set the function to run on the button click in the notification.
     *
     * @param onButtonPress std::fuction or lambda expression, which contains the code for execution.
    */
    inline void SetOnButtonPress(const std::function<bool()>& onButtonPress)
    {
        m_OnPress = onButtonPress;
    }

    bool HasSeperator() const { return m_UseSeperator; }
    bool HasCloseButton() const { return m_HasCloseButton; }
public:
    // Getters

    ImGuiToastPhase GetState() const { return m_State; }

    /**
     * @brief Get the title of the toast notification.
     *
     * @return const char* The title of the toast notification.
     */
    inline std::string_view GetTitle()
    {
        return m_Title;
    };

    /**
     * @brief Get the default title of the toast notification based on its type.
     *
     * @return const char* The default title of the toast notification.
     */
    inline std::string_view GetDefaultTitle()
    {
        if (m_Title.empty())
        {
            switch (m_Type)
            {
            case ImGuiToastType::Success:
                return "Success";
            case ImGuiToastType::Warning:
                return "Warning";
            case ImGuiToastType::Error:
                return "Error";
            case ImGuiToastType::Info:
                return "Info";
            case ImGuiToastType::None:
            default:
                return "";
            }
        }

        return m_Title;
    };

    /**
     * @brief Get the type of the toast notification.
     *
     * @return ImGuiToastType The type of the toast notification.
     */
    inline ImGuiToastType GetType()
    {
        return m_Type;
    };

    /**
     * @brief Get the color of the toast notification based on its type.
     *
     * @return ImVec4 The color of the toast notification.
     */
    inline ImVec4 GetColor()
    {
        switch (m_Type)
        {
        case ImGuiToastType::None:
            return { 255, 255, 255, 255 }; // White
        case ImGuiToastType::Success:
            return { 0, 255, 0, 255 }; // Green
        case ImGuiToastType::Warning:
            return { 255, 255, 0, 255 }; // Yellow
        case ImGuiToastType::Error:
            return { 255, 0, 0, 255 }; // Error
        case ImGuiToastType::Info:
            return { 0, 157, 255, 255 }; // Blue
        default:
            return { 255, 255, 255, 255 }; // White
        }
    }

    /**
     * @brief Get the content of the toast notification.
     *
     * @return char* The content of the toast notification.
     */
    inline std::string_view GetContent()
    {
        return m_Content;
    };

    /**
     * @brief Get the elapsed time in milliseconds since the creation of the object.
     *
     * @return int64_t The elapsed time in milliseconds.
     * @throws An exception with the message "Unsupported platform" if the platform is not supported.
     */
    inline uint64_t GetElapsedTime()
    {
        return Onyx::Time::GetCurrentMilliseconds() - m_CreationTime;
    }

    /**
     * @brief Get the current phase of the toast notification based on the elapsed time since its creation.
     *
     * @return ImGuiToastPhase The current phase of the toast notification.
     *         - ImGuiToastPhase::FadeIn: The notification is fading in.
     *         - ImGuiToastPhase::Wait: The notification is waiting to be dismissed.
     *         - ImGuiToastPhase::FadeOut: The notification is fading out.
     *         - ImGuiToastPhase::Expired: The notification has expired and should be removed.
     */
    inline void Update(ImGuiNotifySettings& settings)
    {
        const uint64_t elapsed = GetElapsedTime();

        if (elapsed > settings.FadeOutAnimationTimeInMilliseconds + m_DismissTime + settings.FadeOutAnimationTimeInMilliseconds)
        {
            m_State = ImGuiToastPhase::Expired;
        }
        else if (elapsed > settings.FadeOutAnimationTimeInMilliseconds + m_DismissTime)
        {
            m_State = ImGuiToastPhase::FadeOut;
        }
        else if (elapsed > settings.FadeOutAnimationTimeInMilliseconds)
        {
            m_State = ImGuiToastPhase::Wait;
        }
        else
        {
            m_State = ImGuiToastPhase::FadeIn;
        }
    }

    /**
     * Returns the percentage of fade for the notification.
     * @return The percentage of fade for the notification.
     */
    inline float GetFadePercent(ImGuiNotifySettings& settings)
    {
        const uint64_t elapsed = GetElapsedTime();

        if (m_State == ImGuiToastPhase::FadeIn)
        {
            return ((float)elapsed / (float)settings.FadeOutAnimationTimeInMilliseconds) * settings.ToastAlpha;
        }
        else
            if (m_State == ImGuiToastPhase::FadeOut)
            {
                return (1.f - (((float)elapsed - (float)settings.FadeOutAnimationTimeInMilliseconds - (float)m_DismissTime) / settings.FadeOutAnimationTimeInMilliseconds)) * settings.ToastAlpha;
            }

        return 1.f * settings.ToastAlpha;
    }

    /**
     * @return ImGui window flags for the notification.
    */
    inline ImGuiWindowFlags GetWindowFlags()
    {
        return m_Flags;
    }

    /**
     * @return The function, which is executed on left click on the notification.
    */
    inline const std::function<bool()>& GetOnPress()
    {
        return m_OnPress;
    }

public:
    // Constructors

    ImGuiToast(ImGuiToastType type, int32_t dismissTime)
        : m_Type(type)
        , m_DismissTime(dismissTime)
    {
        IM_ASSERT(type < ImGuiToastType::COUNT);
    }

    ImGuiToast(ImGuiToastType type, int32_t dismissTime, std::string_view content)
        : ImGuiToast(type, dismissTime)
    {
        m_Content = content;
    }

    ImGuiToast(ImGuiToastType type, int32_t dismissTime, std::string_view content, const std::function<bool()>& onPress)
        : ImGuiToast(type, dismissTime, content)
    {
        m_OnPress = onPress;
    }
};

namespace ImGui
{
    inline std::vector<ImGuiToast> Notifications;
    inline std::recursive_mutex NotificationMutex;
    inline ImGuiNotifySettings NotificationSettings;

    /**
     * Inserts a new notification into the notification queue.
     * @param toast The notification to be inserted.
     */
    inline ImGuiToast& InsertNotification(const ImGuiToast& toast)
    {
        std::scoped_lock lock(NotificationMutex);
        if (NotificationSettings.Order == ImGuiToastOrder::FIFO)
            return Notifications.emplace_back(toast);
        else
            return *Notifications.emplace(Notifications.begin(), toast);
    }

    /**
     * @brief Inserts a new ImGuiToast object with the specified type and dismiss time.
     *
     * @param type The type of the toast.
     * @param dismissTime The time in milliseconds after which the toast should be dismissed. Default is NOTIFY_DEFAULT_DISMISS.
     */
    inline ImGuiToast& InsertNotification(ImGuiToastType type, int32_t dismissTime = NotificationSettings.DefaultAutoDismissInMilliseconds)
    {
        return InsertNotification({ type, dismissTime });
    }

    /**
     * @brief Inserts a new ImGuiToast object with the specified type and dismiss time.
     *
     * @param type The type of the toast.
     * @param dismissTime The time in milliseconds after which the toast should be dismissed. Default is NOTIFY_DEFAULT_DISMISS.
     */
    inline ImGuiToast& InsertNotification(ImGuiToastType type, std::string_view content, int32_t dismissTime = NotificationSettings.DefaultAutoDismissInMilliseconds)
    {
        return InsertNotification({ type, dismissTime, content });
    }

    /**
     * @brief Constructor for creating a new ImGuiToast object with a specified type, dismiss time, title format, content format and a button.
     *
     * @param type The type of the toast message.
     * @param dismissTime The time in milliseconds before the toast message is dismissed.
     * @param content The content of the notification.
     * @param onButtonPress The lambda function to be executed when the button is pressed.
     */
    inline ImGuiToast& InsertNotification(ImGuiToastType type, std::string_view content, const std::function<bool()>& onPress, int32_t dismissTime = NotificationSettings.DefaultAutoDismissInMilliseconds)
    {
        return InsertNotification({ type, dismissTime, content, onPress });
    }

    /**
     * @brief Removes a notification from the list of notifications.
     *
     * @param index The index of the notification to remove.
     */
    inline void RemoveNotification(int index)
    {
        std::scoped_lock lock(NotificationMutex);
        Notifications.erase(Notifications.begin() + index);
    }

    inline void RemoveAllNotifications()
    {
        std::scoped_lock lock(NotificationMutex);
        Notifications.clear();
    }

    /**
     * Renders all notifications in the notifications vector.
     * Each notification is rendered as a toast window with a title, content and an optional icon.
     * If a notification is expired, it is removed from the vector.
     */
    inline void RenderNotifications()
    {
        ImVec2 mainWindowPosition = GetMainViewport()->Pos;
        ImVec2 mainWindowSize = GetMainViewport()->Size;
        if (NotificationSettings.RenderOnScreen)
        {
            short mainMonitorId = static_cast<ImGuiViewportP*>(GetMainViewport())->PlatformMonitor;
            if (mainMonitorId != -1)
            {
                ImGuiPlatformIO& platformIO = GetPlatformIO();
                ImGuiPlatformMonitor& monitor = platformIO.Monitors[mainMonitorId];

                mainWindowPosition = monitor.WorkPos;
                mainWindowSize = monitor.WorkSize;
            }
        }

        const ImGuiStyle& style = GetStyle();

        float height = 0.f;

        std::scoped_lock lock(NotificationMutex);
        for (int32_t i = 0; i < static_cast<int32_t>(Notifications.size()); ++i)
        {
            if (i > NotificationSettings.MaxNotifications)
            {
                break;
            }

            ImGuiToast* currentToast = &Notifications[i];

            if ((NotificationSettings.AutoDismissOnlyVisibleNotifications) && (currentToast->GetState() == ImGuiToastPhase::Queued))
            {
                currentToast->SetCreationTime(Onyx::Time::GetCurrentMilliseconds());
            }

            currentToast->Update(NotificationSettings);

            // Remove toast if expired
            if(currentToast->GetState() == ImGuiToastPhase::Expired)
            {
                RemoveNotification(i);
                continue;
            }

            // Get icon, title and other data
            const char* icon = nullptr;// currentToast->getIcon();
            const std::string_view& title = currentToast->GetTitle();
            const std::string_view& content = currentToast->GetContent();
            const std::string_view& defaultTitle = currentToast->GetDefaultTitle();
            const float opacity = currentToast->GetFadePercent(NotificationSettings); // Get opacity based of the current phase

            const std::function<bool()>& onPressFunctor = currentToast->GetOnPress();
            const std::function<bool()>& customContentFunctor = currentToast->GetCustomContentFunctor();
            // Window rendering
            ImVec4 textColor = currentToast->GetColor();
            textColor.w = opacity;

            // Generate new unique name for this toast
            char windowName[50];
#ifdef _WIN32
            sprintf_s(windowName, "##TOAST%d", i);
#elif defined(__linux__) || defined(__EMSCRIPTEN__)
            std::sprintf(windowName, "##TOAST%d", i);
#elif defined (__APPLE__)
            std::snprintf(windowName, 50, "##TOAST%d", i);
#else
            throw "Unsupported platform";
#endif

            //PushStyleColor(ImGuiCol_Text, textColor);
            SetNextWindowBgAlpha(opacity);
           
            ImVec2 windowPosition = mainWindowPosition;
            ImVec2 pivot(0.0f, 0.0f);
            switch (NotificationSettings.Position)
            {
                case ImGuiToastPos::TopLeft: 
                    pivot = ImVec2(0.0f, 0.0f);
                    windowPosition.x += style.WindowPadding.x;
                    windowPosition.y += style.WindowPadding.y;
                    break;
                case ImGuiToastPos::TopCenter:
                    pivot = ImVec2(0.5f, 0.0f);
                    windowPosition.x += mainWindowSize.x / 2.0f;
                    windowPosition.y += style.WindowPadding.y;
                    break;
                case ImGuiToastPos::TopRight:
                    pivot = ImVec2(1.0f, 0.0f);
                    windowPosition.x += mainWindowSize.x - style.WindowPadding.x;
                    windowPosition.y += style.WindowPadding.y;
                    break;
                case ImGuiToastPos::BottomLeft:
                    pivot = ImVec2(0.0f, 1.0f);
                    windowPosition.x += style.WindowPadding.x;
                    windowPosition.y += mainWindowSize.y - style.WindowPadding.y;
                    break;
                case ImGuiToastPos::BottomCenter:
                    pivot = ImVec2(0.5f, 1.0f);
                    windowPosition.x += mainWindowSize.x / 2.0f;
                    windowPosition.y += mainWindowSize.y - style.WindowPadding.y;
                    break;
                case ImGuiToastPos::BottomRight:
                    pivot = ImVec2(1.0f, 1.0f);
                    windowPosition.x += mainWindowSize.x - style.WindowPadding.x;
                    windowPosition.y += mainWindowSize.y - style.WindowPadding.y;
                    break;
                case ImGuiToastPos::Center:
                    pivot = ImVec2(0.5f, 0.5f);
                    windowPosition.x += mainWindowSize.x / 2.0f;
                    windowPosition.y += mainWindowSize.y / 2.0f;
                    break;
                case ImGuiToastPos::COUNT:
                default:
                    pivot = ImVec2(0.0f, 0.0f);
                    break;
            }

            SetNextWindowSizeConstraints(ImVec2(NotificationSettings.MinSize.x, NotificationSettings.MinSize.y), ImVec2(NotificationSettings.MaxSize.x, NotificationSettings.MaxSize.y));
            SetNextWindowPos(ImVec2(windowPosition.x, windowPosition.y + height), ImGuiCond_Always, pivot);

            // Set notification window flags
            bool hasCloseButton = currentToast->HasCloseButton();
            if ((hasCloseButton == false) && (onPressFunctor == nullptr))
            {
                currentToast->SetWindowFlags(NOTIFY_DEFAULT_TOAST_FLAGS | ImGuiWindowFlags_NoInputs);
            }

            Begin(windowName, nullptr, currentToast->GetWindowFlags() | ImGuiWindowFlags_NoSavedSettings);

            // Render over all other windows
            BringWindowToDisplayFront(GetCurrentWindow());

            // Here we render the toast content
            {
                PushTextWrapPos(mainWindowSize.x / 3.f); // We want to support multi-line text, this will wrap the text after 1/3 of the screen width

                bool wasTitleRendered = false;

                // If an icon is set
                if (icon != nullptr)
                {
                    //Text(icon); // Render icon text
                    TextColored(textColor, "%s", icon);
                    wasTitleRendered = true;
                }

                // If a title is set
                if (title.empty() == false)
                {
                    // If a title and an icon is set, we want to render on same line
                    if (icon != nullptr)
                        SameLine();

                    TextEx(title.data()); // Render title text
                    wasTitleRendered = true;
                }
                else if (defaultTitle.empty() == false)
                {
                    if (icon != nullptr)
                        SameLine();

                    TextEx(defaultTitle.data()); // Render default title text (ImGuiToastType_Success -> "Success", etc...)
                    wasTitleRendered = true;
                    
                }
                // If a dismiss button is enabled
                if (hasCloseButton)
                {
                    // If a title or content is set, we want to render the button on the same line
                    if (wasTitleRendered || (content.empty() == false))
                    {
                        SameLine();
                    }

                    // Render the dismiss button on the top right corner
                    // NEEDS TO BE REWORKED
                    float scale = 0.8f;

                    if (CalcTextSize(content.data()).x > GetContentRegionAvail().x)
                    {
                        scale = 0.8f;
                    }

                    SetCursorPosX(GetCursorPosX() + (GetWindowSize().x - GetCursorPosX()) * scale);

                    const float iconSize = GetTextLineHeightWithSpacing() - (GetStyle().FramePadding.y / 2.0f);
                    const float halfIconSize = iconSize / 3.0f;
                    
                    // If the button is pressed, we want to remove the notification
                    if (Onyx::Ui::DrawCloseButton(GetWindowDrawList(), ImVec2(0.0f, 0.0f), halfIconSize, 0xC8A0A0A0))
                    {
                        RemoveNotification(i);
                        End();
                        continue;
                    }
                }
                
                if (onPressFunctor != nullptr)
                {
                    bool isHovered;
                    bool isHeld;
                    ImRect bounds (GetWindowContentRegionMin(), GetWindowContentRegionMax());
                    bounds.Min.x += GetWindowPos().x;
                    bounds.Min.y += GetWindowPos().y;
                    bounds.Max.x += GetWindowPos().x;
                    bounds.Max.y += GetWindowPos().y;

                    ImGuiID closeAreaId = GetID("##closeArea");
                    if (ButtonBehavior(bounds, closeAreaId, &isHovered, &isHeld, static_cast<ImGuiButtonFlags>(ImGuiButtonFlags_PressedOnClick) | ImGuiButtonFlags_MouseButtonMask_))
                    {
                        bool shouldClose = onPressFunctor();
                        if (shouldClose)
                        {
                            // do a proper remove and change the loop to iterate from the back to front
                            if (Notifications.empty() == false)
                                RemoveNotification(i);

                            End();
                            continue;
                        }
                    }
                    ImGui::KeepAliveID(closeAreaId);
                }

                // In case ANYTHING was rendered in the top, we want to add a small padding so the text (or icon) looks centered vertically
                if (wasTitleRendered && (content.empty() == false))
                {
                    SetCursorPosY(GetCursorPosY() + 5.0f); // Must be a better way to do this!!!!
                }

                // If a content is set
                if (content.empty() == false)
                {
                    if (wasTitleRendered && currentToast->HasSeperator())
                    {
                        Separator();
                    }
                    
                    ImGui::PushTextWrapPos(GetCursorPos().x + GetContentRegionAvail().x - style.FramePadding.x);
                    ImGui::Text("%s", content.data());
                    ImGui::PopTextWrapPos();
                }

                // If a button is set
                if (customContentFunctor != nullptr)
                {
                    bool shouldClose = customContentFunctor();
                    if (shouldClose)
                    {
                        RemoveNotification(i);
                        End();
                        continue;
                    }
                }
            }

            // Save height for next toasts
            if (NotificationSettings.Direction == ImGuiToastDirection::Up)
                height -= GetWindowHeight() + NotificationSettings.VerticalMessagePadding;
            else
                height += GetWindowHeight() + NotificationSettings.VerticalMessagePadding;

            // End
            End();
        }
    }
}

#endif

#endif
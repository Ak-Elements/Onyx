#include <onyx/platform/linux/wayland/waylandwindow.h>

#if ONYX_IS_LINUX && ONYX_USE_WAYLAND

#include <onyx/log/logger.h>
#include <onyx/platform/linux/wayland/waylandplatformcontext.h>

#include <wayland-client-protocol.h>
#include <wayland-xdg-shell-client-protocol.h>
#include <wayland-xdg-decoration-client-protocol.h>

///
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
///

namespace onyx::platform::wayland
{
    // TODO: Remove
namespace
{
static void
randname(char *buf)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	long r = ts.tv_nsec;
	for (int i = 0; i < 6; ++i) {
		buf[i] = 'A'+(r&15)+(r&16)*2;
		r >>= 5;
	}
}

static int
create_shm_file(void)
{
	int retries = 100;
	do {
		char name[] = "/wl_shm-XXXXXX";
		randname(name + sizeof(name) - 7);
		--retries;
		int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
		if (fd >= 0) {
			shm_unlink(name);
			return fd;
		}
	} while (retries > 0 && errno == EEXIST);
	return -1;
}

int
allocate_shm_file(size_t size)
{
	int fd = create_shm_file();
	if (fd < 0)
		return -1;
	int ret;
	do {
		ret = ftruncate(fd, size);
	} while (ret < 0 && errno == EINTR);
	if (ret < 0) {
		close(fd);
		return -1;
	}
	return fd;
}
}

    Window::Window(WaylandPlatformContext& context, const WindowSettings& settings)
        : m_Settings(settings)
        , m_Context(&context)
    {
        CreateNativeWindow();
    }

    Window::~Window()
    {
    }

    void Window::Show()
    {
    }

    void Window::Hide()
    {
    }

    void Window::Minimize()
    {
    }

    void Window::Maximize()
    {
    }

    void Window::CreateNativeWindow()
    {
        static const struct xdg_toplevel_listener xdg_toplevel_listener = {
            HandleToplevelConfigure,
            HandleToplevelClose,
        };

        static const struct xdg_surface_listener xdg_surface_listener = { HandleSurfaceConfigure, };
        //static const struct zxdg_toplevel_decoration_v1_listener zxdgTopLevelDecorationListener = { HandleTopLevelDecorationConfigure, };
        
        static const struct wl_surface_listener wl_surface_listener =
        {
            HandleSurfaceEnter,
            HandleSurfaceLeave
        };

        m_Surface = ::wl_compositor_create_surface(m_Context->GetCompositiorHandle());
        wl_surface_add_listener(m_Surface, &wl_surface_listener, this);
        m_XdgSurface = ::xdg_wm_base_get_xdg_surface(m_Context->GetShellHandle(), m_Surface);
        ::xdg_surface_add_listener(m_XdgSurface, &xdg_surface_listener, this);

        m_XdgToplevel = ::xdg_surface_get_toplevel(m_XdgSurface);
        ::xdg_toplevel_add_listener(m_XdgToplevel, &xdg_toplevel_listener, this);

        ::xdg_toplevel_set_app_id(m_XdgToplevel, "onyx");
        ::xdg_toplevel_set_title(m_XdgToplevel, m_Settings.Title.c_str());
        
        //if (m_Settings.Mode == WindowMode::Fullscreen)
        {
      //      ::xdg_toplevel_set_fullscreen(m_XdgToplevel, NULL);
        }

        ::wl_surface_commit(m_Surface);
        ::wl_display_flush(m_Context->GetDisplayHandle());

        m_XdgTopLevelDecoration = ::zxdg_decoration_manager_v1_get_toplevel_decoration(m_Context->GetDecorationManager(), m_XdgToplevel);
        zxdg_toplevel_decoration_v1_set_mode(m_XdgTopLevelDecoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE);
        
        wl_surface_commit(m_Surface);
        wl_display_roundtrip(m_Context->GetDisplayHandle());

        const int width = 1920, height = 1080;
        const int stride = width * 4;
        const int shm_pool_size = height * stride * 2;

        int fd = allocate_shm_file(shm_pool_size);
        uint8_t* pool_data = reinterpret_cast<uint8_t*>(mmap(NULL, shm_pool_size,
            PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

       // struct wl_shm *shm = ...; // Bound from registry
        struct wl_shm_pool *pool = wl_shm_create_pool(m_Context->GetSharedMemory(), fd, shm_pool_size);

        int index = 0;
        int offset = height * stride * index;
        struct wl_buffer* buffer = wl_shm_pool_create_buffer(pool, offset,
            width, height, stride, WL_SHM_FORMAT_XRGB8888);
        
        uint32_t* pixels = (uint32_t *)&pool_data[offset];
        memset(pixels, 0xcc, width * height * 4);
        
        wl_surface_attach(m_Surface, buffer, 0, 0);
        wl_surface_damage(m_Surface, 0, 0, UINT32_MAX, UINT32_MAX);
        wl_surface_commit(m_Surface);
    }

    void Window::SetSize(onyxS32 width, onyxS32 height)
    {
        if ((width != m_Settings.Size[0]) || (height != m_Settings.Size[1]))
        {
            m_Settings.Size[0] = width;
            m_Settings.Size[1] = height;

            m_ResizeSignal.Dispatch(m_Settings.Size);
        }
    }

    void Window::SetMinimumSize(const Vector2s32& minSize)
    {
        if (m_Settings.MinSize != minSize)
        {
            m_Settings.MinSize = minSize;
            VectorComponentMask compared = m_Settings.Size > m_Settings.MinSize;
            SetSize(enums::HasAllFlags(compared, VectorComponentMask::X)  ? m_Settings.MinSize[1] : m_Settings.Size[0], enums::HasAllFlags(compared, VectorComponentMask::Y) ? m_Settings.MinSize[1] : m_Settings.Size[1]);
        }
    }

    void Window::SetMaximumSize(const Vector2s32& maxSize)
    {
        if (m_Settings.MaxSize != maxSize)
        {
            m_Settings.MaxSize = maxSize;
            VectorComponentMask compared = m_Settings.Size > m_Settings.MaxSize;
            SetSize(enums::HasAllFlags(compared, VectorComponentMask::X) ? m_Settings.MaxSize[1] : m_Settings.Size[0], enums::HasAllFlags(compared, VectorComponentMask::Y) ? m_Settings.MaxSize[1] : m_Settings.Size[1]);
        }
    }

    void Window::SetWindowMode(WindowMode mode)
    {
        //if (m_Settings.m_Mode == mode)
        //    return;

        m_Settings.Mode = mode;
    }

    void Window::SetState(WindowState state)
    {
        if (m_State != state)
        {
            switch (state)
            {
                case WindowState::None:
                case WindowState::Hidden:
                {
                    break;
                }
                case WindowState::Minimized:
                {
                    Minimize();
                    break;
                }
                case WindowState::Background:
                {
                    break;
                }
                case WindowState::Default:
                {
                    break;
                }
                case WindowState::Maximized:
                {
                    break;
                }
                case WindowState::Resizing:
                {
                    break;
                }
            }

            m_State = state;
        }
    }

    bool Window::GetRequiredExtensions(std::vector<const char*>& outExtensions) const
    {
        outExtensions.push_back("VK_KHR_wayland_surface");
        return true;
    }

    /*static*/ void Window::HandleSurfaceConfigure(void* data, xdg_surface* surface, onyxU32 serial)
    {
        Window& windowInstance = *static_cast<Window*>(data);
        xdg_surface_ack_configure(surface, serial);

        if (windowInstance.m_IsInitialized)
        {
            wl_surface_commit(windowInstance.GetSurfaceHandle());
            wl_display_flush(windowInstance.GetContext()->GetDisplayHandle());
        }

        windowInstance.m_IsInitialized = true;
        windowInstance.m_IsInitialized.notify_one();
    }

    /*static*/ void Window::HandleToplevelConfigure(void* data, xdg_toplevel* toplevel, onyxS32 width, onyxS32 height, wl_array* states)
    {
        Window& windowInstance = *static_cast<Window*>(data);
        windowInstance.SetSize(width, height);
    }

    /*static*/ void Window::HandleToplevelClose(void* data, xdg_toplevel* xdg_toplevel)
    {
        // do not hide, destroy window instead
        Window& windowInstance = *static_cast<Window*>(data);
        windowInstance.Hide();
    }

    /*static*/ void Window::HandleTopLevelDecorationConfigure(void *data, zxdg_toplevel_decoration_v1* zxdg_toplevel_decoration_v1, onyxU32 mode)
    {
        ONYX_UNUSED(data);
        ONYX_UNUSED(zxdg_toplevel_decoration_v1);
        ONYX_UNUSED(mode);
    }

    /*static*/ void Window::HandleSurfaceEnter(void* data, wl_surface* surface, wl_output* output)
    {
        ONYX_UNUSED(data);
        ONYX_UNUSED(surface);
        ONYX_UNUSED(output);
    }

    /*static*/ void Window::HandleSurfaceLeave(void* data, wl_surface* surface, wl_output* output)
    {
        ONYX_UNUSED(data);
        ONYX_UNUSED(surface);
        ONYX_UNUSED(output);
    }

}
#endif
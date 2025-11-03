#include <onyx/application/application.h>
#include <onyx/filesystem/jsondeserializer.h>
#include <onyx/filesystem/onyxfile.h>

namespace Onyx::Application
{
    extern void OnApplicationCreate();
    extern void OnApplicationCreated(Application& application);
    extern void OnApplicationShutdown(Application& application);
}

void* operator new(size_t size)
{
    //cout << "New operator overloading " << endl;
    void* p = malloc(size);
    return p;
}

void operator delete(void* p) noexcept
{
    //cout << "Delete operator overloading " << endl;
    free(p);
}


#if 1
int main(int, char**)
#else
int CALLBACK WinMain(
    HINSTANCE   /*hInstance*/,
    HINSTANCE   /*hPrevInstance*/,
    LPSTR       /*lpCmdLine*/,
    int         /*nCmdShow*/
)
#endif
{
    using namespace Onyx::Application;


    OnApplicationCreate();

    Application application;
    application.Init();
    application.Run();
    application.Shutdown();

    return 0;
}

#ifndef __MAIN_HH_
#define __MAIN_HH_

#include "hudkit_window.hh"
#include "include/cef_base.h"
#include "include/cef_app.h"

class Hudkit : public CefApp, public CefBrowserProcessHandler
{
public:
    Hudkit();
    virtual ~Hudkit();

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
        OVERRIDE
    {
        return this;
    }

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() OVERRIDE;
    void Run();

private:
    HudkitWindow hudkitWindow;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(Hudkit);
};

#endif

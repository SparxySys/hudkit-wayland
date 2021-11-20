#ifndef __RENDER_HANDLER_HH_
#define __RENDER_HANDLER_HH_

#include "draw.hh"
#include "include/cef_browser.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

class HudkitRenderHandler : public CefRenderHandler
{
public:
  HudkitRenderHandler(DrawArea draw);

  void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;
  void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) override;

private:
  IMPLEMENT_REFCOUNTING(HudkitRenderHandler);

protected:
  DrawArea draw;
};

#endif
#ifndef __BROWSER_HANDLER_H_
#define __BROWSER_HANDLER_H_

#include "include/base/cef_callback.h"
#include "include/cef_client.h"
#include "render_handler.hh"

#include <list>

class BrowserHandler : public CefClient, public CefDisplayHandler, public CefLifeSpanHandler, public CefLoadHandler {
 public:
  explicit BrowserHandler(bool use_views, CefRefPtr<CefRenderHandler> renderHandler);

  // Provide access to the single global instance of this object.
  static BrowserHandler* GetInstance();

  // CefClient methods:
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override
  {
    return this;
  }
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override
  {
    return this;
  }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override
  {
    return this;
  }

  virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override
  {
    return m_renderHandler;
  }

  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

  virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           ErrorCode errorCode,
                           const CefString& errorText,
                           const CefString& failedUrl) override;

  virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int httpStatusCode) override;

  void CloseAllBrowsers(bool force_close);

  void Refresh();

  bool IsClosing() const { return is_closing_; }

 protected:
  ~BrowserHandler();

 private:
  const bool use_views_;

  typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
  BrowserList browser_list_;

  bool is_closing_;

  CefRefPtr<CefRenderHandler> m_renderHandler;

  IMPLEMENT_REFCOUNTING(BrowserHandler);
};

#endif

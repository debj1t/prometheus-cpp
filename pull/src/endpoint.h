#pragma once

#include <functional>
#include <memory>
#include <string>

#include "CivetServer.h"
#include "basic_auth.h"
#include "handler.h"
#include "prometheus/collectable.h"
#include "prometheus/registry.h"
#include "prometheus/http_server.h"

namespace prometheus {
namespace detail {

class Endpoint {
 public:
  explicit Endpoint(HttpServer& server, std::wstring uri);
  ~Endpoint();

  Endpoint(const Endpoint&) = delete;
  Endpoint(Endpoint&&) = delete;
  Endpoint& operator=(const Endpoint&) = delete;
  Endpoint& operator=(Endpoint&&) = delete;

  void RegisterCollectable(const std::weak_ptr<Collectable>& collectable);
//  void RegisterAuth(
//      std::function<bool(const std::string&, const std::string&)> authCB,
//      const std::string& realm);
  void RemoveCollectable(const std::weak_ptr<Collectable>& collectable);

  const std::wstring& GetURI() const;

 private:
  HttpServer& server_;
  const std::wstring uri_;
  // registry for "meta" metrics about the endpoint itself
  std::shared_ptr<Registry> endpoint_registry_;
  std::unique_ptr<MetricsHandler> metrics_handler_;
  std::unique_ptr<BasicAuthHandler> auth_handler_;
};

}  // namespace detail
}  // namespace prometheus

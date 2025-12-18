#pragma once
#include "oatpp/core/macro/component.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"

class AppComponent {
public:
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::tcp::server::ConnectionProvider>, connectionProvider)([] {
    return oatpp::network::tcp::server::ConnectionProvider::createShared({"0.0.0.0", 8000, oatpp::network::Address::IP_4});
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router)([] {
    return oatpp::web::server::HttpRouter::createShared();
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpConnectionHandler>, connectionHandler)([] {
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
    return oatpp::web::server::HttpConnectionHandler::createShared(router);
  }());
};

#include "oatpp/core/base/Environment.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/core/async/Executor.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "AppComponent.hpp"
#include "BookController.hpp"

int main() {
  oatpp::base::Environment::init();

  AppComponent components;
  OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

  auto objectMapper = oatpp::parser::json::mapping::ObjectMapper::createShared();
  objectMapper->getSerializer()->getConfig()->useBeautifier = true;
  auto controller = std::make_shared<BookController>(objectMapper);
  router->addController(controller);


  OATPP_COMPONENT(std::shared_ptr<oatpp::network::tcp::server::ConnectionProvider>, connectionProvider);
  OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpConnectionHandler>, connectionHandler);

  oatpp::network::Server server(connectionProvider, connectionHandler);
  OATPP_LOGI("LibraryWeb", "Server running on port 8000");
  server.run();

  oatpp::base::Environment::destroy();
  return 0;
}
